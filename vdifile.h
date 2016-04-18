#ifndef VDIFILE_H
#define VDIFILE_H

#include <QObject>
#include <QFile>
#include "vdimap.h"
#include "mbrdata.h"
#include "ext2superblock.h"
#include "ext2groupdescriptor.h"
#include <fstream>
#include <vector>

struct VdiHeader {
long long headerSize;
long long imageType;
long long offsetBlocksSize;
long long offsetDataSize;
long long sectorSize;
long long diskSize;
long long blockSize;
long long blocksInHDD;
long long blocksAllocatedSize;
long long UUIDofVDI;
long long UUIDofSNAP;
};

struct InodeTable {
    unsigned short i_mode;
    unsigned short i_uid;
    unsigned int i_size;
    unsigned int i_atime;
    unsigned int i_ctime;
    unsigned int i_mtime;
    unsigned int i_dtime;
    unsigned short i_gid;
    unsigned short i_links_count;
    unsigned int i_blocks;
    unsigned int i_flags;
    unsigned int i_osd1;
    unsigned int i_block[15];
    unsigned int i_generation;
    unsigned int i_file_acl;
    unsigned int i_dir_acl;
    unsigned int i_faddr;
    unsigned char i_osd2[12];

};


class VdiFile : public QObject
{
    Q_OBJECT
public:
    VdiFile(QObject *parent = 0);
    ~VdiFile();

    void selectVdiPrompt();
    void openFile(QString fileName);


public slots:

signals:
    void vdiFileSelected(QString fileName);

private:
    void closeAndReset();
    void getHeaderValues();
    //In globalfunctions.h//unsigned char getCharFromStream(int size, long long seek_to, std::ifstream &input);
    void fillDataBlockBitmap(QVector<unsigned char>* DataBlockBitmap, unsigned int block_bitmap_address,unsigned int inode_bitmap_address,std::ifstream& input);
    void getInodeTableData(long long beginningOfFirstInodeTable, int InodeNumber, std::ifstream &file);
    void getDataBlock(long long BlockNumber, std::ifstream &file);

    QFile *vdi; //whatever filetype we intend to use
    VdiMap *map;
    mbrData *mbr;
    ext2SuperBlock *superBlock;
    std::ifstream input;
    //QVector<ext2GroupDescriptor*> *groupDescriptors;
    ext2GroupDescriptor *groupDescriptors;
    VdiHeader header;
    InodeTable tab;
    QVector<unsigned char> *DataBlockBitmap;

    std::vector<bool> *blockBitmap, *inodesBitmap;
    //std::vector optimized bool storage to take 1 bit per boolean value when there are multiple bools

    unsigned int group_size;
    unsigned int superBlockLocation;
     unsigned int block_size;
     unsigned int bootBlockLocation;



};

#endif // VDIFILE_H
