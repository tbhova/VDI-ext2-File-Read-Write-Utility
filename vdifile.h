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
#include <cstring>
#include "ext2filesystemmanager.h"

namespace CSCI5806 {
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



class VdiFile : public QObject
{
    Q_OBJECT
public:
    VdiFile(QObject *parent = 0);
    ~VdiFile();
    void openFile(QString fileName);


public slots:
    void selectVdiPrompt();

signals:
    void vdiFileSelected(QString fileName);
    void FSManagerConstructed(ext2FileSystemManager *fs);

private:
    void closeAndReset();
    void getHeaderValues();
    //In globalfunctions.h//unsigned char getCharFromStream(int size, long long seek_to, std::ifstream &input);
    void fillDataBlockBitmap(QVector<unsigned char>* DataBlockBitmap, unsigned int block_bitmap_address,unsigned int inode_bitmap_address,std::ifstream& input);
    void getInodeTableData(long long beginningOfFirstInodeTable, int InodeNumber, std::ifstream &file);
    void getDataBlock(long long BlockNumber, std::ifstream &file);
    void fillRootDir(long long block_num,long long offsetOfStruct, std::ifstream &file);

    QFile *vdi; //whatever filetype we intend to use
    VdiMap *map;
    mbrData *mbr;
    ext2SuperBlock *superBlock;
    std::ifstream input;
    //QVector<ext2GroupDescriptor*> *groupDescriptors;
    ext2GroupDescriptor *groupDescriptors;
    VdiHeader header;
    InodeTable tab;
    Inode_info InodeIn;
    QVector<unsigned char> *DataBlockBitmap;
    QVector <Inode_info> *InodeInfo;
    ext2FileSystemManager *fsManager;

    std::vector<bool> *blockBitmap, *inodesBitmap;
    //std::vector optimized bool storage to take 1 bit per boolean value when there are multiple bools

    unsigned int bootBlockLocation, superBlockLocation, block_size, group_size;
};
}

#endif // VDIFILE_H
