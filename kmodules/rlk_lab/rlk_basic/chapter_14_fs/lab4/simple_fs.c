#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/genhd.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/uaccess.h>
#include <linux/version.h>


#define FILE_NAME			64
#define MAX_BLOCKS_NUM		64
#define PER_BLOCK_SIZE		512

#define SUPER_BLOCK_IDX		0
#define FIRST_FILE_BLOCK_IDX  1

static int exist_file_num;

struct simple_dir_entry {
	char filename[FILE_NAME];
	unsigned int idx;
};

struct block_info {
	unsigned char in_use;
	mode_t mode;
	unsigned int idx;
	unsigned int file_size;
	unsigned int dir_children;
	void *data;
};

struct block_info blocks[MAX_BLOCKS_NUM + 1];

static int get_block(void)
{
	int i;
	for (i = FIRST_FILE_BLOCK_IDX; i < MAX_BLOCKS_NUM + 1; i++) {
		if (!blocks[i].in_use) {
			blocks[i].in_use = 1;
			return i;
		}
	}

	return -EBUSY;
}

static ssize_t simple_fs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	struct block_info *blk = (struct block_info *)inode->i_private;
	char *blk_buf;

	if (*ppos >= blk->file_size)
		return 0;

	blk_buf = (char *)blk->data;
	blk_buf += *ppos;
	len = min((size_t)blk->file_size, len);
	if (copy_to_user(buf, blk_buf, len)) {
		return -EFAULT;
	}
	*ppos += len;

	return len;
}

static ssize_t simple_fs_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	struct block_info *blk = (struct block_info *)inode->i_private;
	char *blk_buf;

	blk_buf = (char *)blk->data;
	blk_buf += *ppos;
	len = min_t(size_t, PER_BLOCK_SIZE, len);
	if (copy_from_user(blk_buf, buf, len)) {
		return -EFAULT;
	}
	*ppos += len;
	blk->file_size = *ppos;

	return len;
}

const struct file_operations simple_fs_file_operations = {
	.owner = THIS_MODULE,
	.read = simple_fs_read,
	.write = simple_fs_write,
};

static int simple_fs_readdir_iter(struct file *filp, struct dir_context *ctx)
{
	int i;
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	struct block_info *blk = (struct block_info *)inode->i_private;
	struct simple_dir_entry*simple_entry;

	if (!S_ISDIR(blk->mode)) {
		return -ENOTDIR;
	}

	if (ctx->pos)
		return 0;

	simple_entry = (struct simple_dir_entry *)blk->data;
	for (i = 0; i < blk->dir_children; i++) {
		if (!dir_emit(ctx, simple_entry[i].filename, strnlen(simple_entry[i].filename, FILE_NAME), 
							simple_entry[i].idx, DT_UNKNOWN)) {
			return 0;
		}
		ctx->pos++;					
	}
	
	return 0;
}

const struct file_operations simple_fs_dir_operations = {
	.owner = THIS_MODULE,
	.iterate_shared = simple_fs_readdir_iter,
};

static const struct inode_operations simple_fs_inode_operations;
static int simple_fs_do_create(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	struct super_block *sb;
	struct inode *inode;
	unsigned int idx;
	struct simple_dir_entry*simple_entry;
	struct block_info *blk, *parent_blk;
	
	sb = dir->i_sb;
	if (exist_file_num >= MAX_BLOCKS_NUM) {
		return -ENOSPC;
	}

	if (!S_ISDIR(mode) && !S_ISREG(mode)) {
		return -EINVAL;
	}

	inode = new_inode(sb);
	if (inode == NULL)
		return -ENOMEM;

	inode->i_sb = sb;
	inode->i_op = &simple_fs_inode_operations;
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	idx = get_block();
	inode->i_ino = idx;
	blk = &blocks[idx];
	blk->mode = mode;
	exist_file_num++;

	if (S_ISDIR(mode) ) {
		blk->dir_children = 0;
		inode->i_fop = &simple_fs_dir_operations;
		set_nlink(inode, 2);
	}else if (S_ISREG(mode)) {
		blk->file_size = 0;
		inode->i_fop = &simple_fs_file_operations;
	}

	inode->i_private = blk;
	parent_blk = (struct block_info *)dir->i_private;
	simple_entry = (struct simple_dir_entry *)parent_blk->data;
	simple_entry += parent_blk->dir_children;
	parent_blk->dir_children++;
	inc_nlink(dir);
	memset(simple_entry, 0, sizeof (struct simple_dir_entry));
	simple_entry->idx = idx;
	strcpy(simple_entry->filename, dentry->d_name.name);

	inode_init_owner(inode, dir, mode);
	d_add(dentry, inode);

	return 0;
}

static int simple_fs_mkdir(struct inode *inode, struct dentry *dentry, umode_t mode)
{
	return simple_fs_do_create(inode, dentry, S_IFDIR | mode);
}

static int simple_fs_create(struct inode *inode, struct dentry *dentry, umode_t mode,
		bool excl)
{
	return simple_fs_do_create(inode, dentry, mode);
}

static struct inode *simple_fs_iget(struct super_block *sb, int idx)
{
	struct inode *inode;
	struct block_info *blk;

	inode = new_inode(sb);
	inode->i_sb = sb;
	inode->i_op = &simple_fs_inode_operations;

	blk = &blocks[idx];
	if (S_ISDIR(blk->mode) ) {
		inode->i_fop = &simple_fs_dir_operations;
	}else if (S_ISREG(blk->mode)) {
		inode->i_fop = &simple_fs_file_operations;
	}
	
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode); 
	inode->i_private = blk;

	return inode;
}

static struct dentry * simple_fs_lookup(struct inode *dir, struct dentry *dentry, unsigned int flags)
{
	int i;
	struct super_block *sb = dir->i_sb;
	struct simple_dir_entry*simple_entry;
	struct block_info *blk;

	blk = (struct block_info *)dir->i_private;
	simple_entry = (struct simple_dir_entry*)blk->data;

	for (i = 0; i < blk->dir_children; i++) {
		if (strcmp(simple_entry[i].filename, dentry->d_name.name) == 0) {
			struct inode *inode = simple_fs_iget(sb, simple_entry[i].idx);
			struct block_info *tmp_inode = (struct block_info *)inode->i_private;

			inode_init_owner(inode, dir, tmp_inode->mode);
			d_add(dentry, inode);
			return NULL;
		}
	}
	return NULL;
}

static int simple_fs_rmdir(struct inode *dir, struct dentry *dentry)
{
	int i, j;
	struct simple_dir_entry *simple_entry;
	struct block_info *blk = (struct block_info *)dentry->d_inode->i_private;
	struct block_info *parent_blk =  (struct block_info *)dir->i_private;

	simple_entry = (struct simple_dir_entry *)parent_blk->data;
	for (i = 0; i < parent_blk->dir_children; i++) {
		if (strcmp(simple_entry[i].filename, dentry->d_name.name) == 0) {
			for (j = i; j < (parent_blk->dir_children - 1); j++) {
				memcpy(&simple_entry[j], &simple_entry[j + 1], sizeof (struct simple_dir_entry));
			}
			parent_blk->dir_children--;
			break;
		}
	}
	blk->in_use = 0;

	return simple_rmdir(dir, dentry);
}

static int simple_fs_unlink(struct inode *dir, struct dentry *dentry)
{
	int i, j;
	struct simple_dir_entry *simple_entry;
	struct inode *inode = dentry->d_inode;
	struct block_info *blk = (struct block_info *)inode->i_private;
	struct block_info *parent_blk =  (struct block_info *)dir->i_private;

	simple_entry = (struct simple_dir_entry *)parent_blk->data;
	for (i = 0; i < parent_blk->dir_children; i++) {
		if (strcmp(simple_entry[i].filename, dentry->d_name.name) == 0) {
			for (j = i; j < (parent_blk->dir_children - 1); j++) {
				memcpy(&simple_entry[j], &simple_entry[j + 1], sizeof (struct simple_dir_entry));
			}
			parent_blk->dir_children--;
			break;
		}
	}
	blk->in_use = 0;

	return simple_unlink(dir, dentry);
}

static const struct inode_operations simple_fs_inode_operations = {
	.create 	= simple_fs_create,
	.lookup 	= simple_fs_lookup,
	.mkdir		= simple_fs_mkdir,
	.rmdir		= simple_fs_rmdir,
	.unlink 	= simple_fs_unlink,
};

static int simple_fs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root_inode;

	root_inode = new_inode(sb);
	root_inode->i_ino = SUPER_BLOCK_IDX;
	inode_init_owner(root_inode, NULL, S_IFDIR);
	root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = current_time(root_inode);
	root_inode->i_op = &simple_fs_inode_operations;
	root_inode->i_fop = &simple_fs_dir_operations;

	blocks[SUPER_BLOCK_IDX].mode = S_IFDIR;
	blocks[SUPER_BLOCK_IDX].in_use = 1;
	blocks[SUPER_BLOCK_IDX].idx = SUPER_BLOCK_IDX;
	blocks[SUPER_BLOCK_IDX].dir_children = 0;
	root_inode->i_private = &blocks[SUPER_BLOCK_IDX];
	
	sb->s_root = d_make_root(root_inode);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}

static struct dentry *
simple_fs_mount(struct file_system_type *fs_type,
	      int flags, const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, simple_fs_fill_super);
}

struct file_system_type simple_fs_type = {
	.owner = THIS_MODULE,
	.name = "simple_fs",
	.mount = simple_fs_mount,
	.kill_sb = kill_anon_super
};

static int __init simple_fs_init(void)
{
	int i, ret;

	for (i = 0; i < MAX_BLOCKS_NUM + 1; i++) {
		blocks[i].data = kzalloc(PER_BLOCK_SIZE, GFP_KERNEL);
	}

	ret = register_filesystem(&simple_fs_type);
	if (ret) {
		pr_err("register simple fs failed\n");
		return ret;
	}
	return 0;
}

static void __exit simple_fs_exit(void)
{
	int i;
	unregister_filesystem(&simple_fs_type);

	for (i = 0; i < MAX_BLOCKS_NUM + 1; i++) {
		kfree(blocks[i].data);
	}
}

module_init(simple_fs_init);
module_exit(simple_fs_exit);

MODULE_AUTHOR("XXXX");
MODULE_LICENSE("GPL v2");


