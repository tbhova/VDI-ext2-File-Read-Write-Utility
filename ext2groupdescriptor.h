#ifndef EXT2GROUPDESCRIPTOR_H
#define EXT2GROUPDESCRIPTOR_H

#include <fstream>
#include <QObject>

namespace CSCI5806 {

/* structure for one group descriptor, we will build a vector of these in the class below
 *
 * */
struct ext2_group_desc
{
unsigned int bg_block_bitmap;			 /* Blocks bitmap block */
unsigned int bg_inode_bitmap; 		     /* Inodes bitmap block */
unsigned int bg_inode_table; 			 /* Inodes table block */
unsigned short bg_free_blocks_count; 	 /* Free blocks count */
unsigned short bg_free_inodes_count; 	 /* Free inodes count */
unsigned short bg_used_dirs_count; 		 /* Directories count */
unsigned short bg_pad;
unsigned int bg_reserved[3];
};

/* holds and builds a vector of ext2_group_desc
 * builds the vector in the constructor given the input file and group descriptor table offset
 * reads the group descriptor table from the 1st block group
 *
 * */
class ext2GroupDescriptor : public QObject
{
    Q_OBJECT
public:
    explicit ext2GroupDescriptor(QObject *parent, int group_count, long long offset, std::fstream &file);
    ~ext2GroupDescriptor();

    //index into group descriptor vector and get the corresponding property
    int getBlockBitmap(int block);
    int getInodeBitmap(int block);
    int getInodeTable(int block);

signals:

public slots:

private:
    ext2_group_desc blockGroup;
    QVector<ext2_group_desc> *groupDescriptors;

};

}

#endif // EXT2GROUPDESCRIPTOR_H
