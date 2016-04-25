#include "vdifile.h"
#include <QMessageBox>
#include <QFileDialog>
#include <string>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog.h>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <qmessagebox.h>
#include <iostream>
#include <String>
#include <iostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <cmath>
#include <sstream>
#include <string>
#include "vdifunctions.h"
#include <QDebug>

using namespace std;
using namespace CSCI5806;

VdiFile::VdiFile(QObject *parent) : QObject(parent)
{
    vdi = new QFile();
    map = NULL;
    mbr = NULL;
    superBlock = NULL;
    groupDescriptors = NULL;
    fsManager = NULL;
    DataBlockBitmap = new QVector<unsigned char>;
    blockBitmap = new vector<bool>;
    inodesBitmap = new vector<bool>;
}

VdiFile::~VdiFile() {
#warning delete everything allocated with new

    delete vdi;
    if (map != NULL)
        delete map;
    if (mbr != NULL)
        delete mbr;
    if (superBlock != NULL)
        delete superBlock;
    if (groupDescriptors != NULL)
        delete groupDescriptors;
    if (DataBlockBitmap != NULL)
        delete DataBlockBitmap;
    if (blockBitmap != NULL)
        delete blockBitmap;
    if (inodesBitmap != NULL)
        delete inodesBitmap;
    if (fsManager != NULL)
        delete fsManager;
}


void VdiFile::selectVdiPrompt() {
    //Open File
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Please open a .vdi file"), "./", ".VDI File (*.*);;All Files (*.*)");

    QMessageBox::information(NULL,tr("FileNameOut"),fileName);
    emit(this->vdiFileSelected(fileName));
    this->openFile(fileName);
}

void VdiFile::openFile(QString fileName) {
    static bool initialized = false; //says we already have a file open
    if (initialized)
        this->closeAndReset();

    qDebug() << QObject::tr("vdi file ") << fileName;

    if (fileName.isEmpty() || fileName.isNull() || !(fileName.toLower()).contains(".vdi")) {
        QMessageBox::information(NULL,tr("Error"), tr("Please choose a valid vdi file."));
        return;
    }


    input.open(fileName.toStdString().c_str(), ios::binary);



    if(!input.is_open()) {
        cout << "File not open!" << endl;
        return;
    }
    input >> noskipws;

    initialized = true;

    header.headerSize = getStreamData((int)4,(long long)72, input,"",true,false,false);
    header.imageType = getStreamData(4,76, input,"",true,false,false);
    header.offsetBlocksSize= getStreamData(4,340,input,"",true,false,false);
    header.offsetDataSize = getStreamData(4, 344, input,"",true,false,false);
    header.sectorSize = getStreamData(4, 360, input ,"",true,false,false);
    header.diskSize = getStreamData (8, 368, input,"",true,false,false);
    header.blockSize = getStreamData (4, 376, input,"",true,false,false);
    header.blocksInHDD = getStreamData(4, 384, input ,"",true,false,false);
    header.blocksAllocatedSize = getStreamData (4, 388, input,"",true,false,false);
    header.UUIDofVDI = getStreamData (16, 392, input,"",true,false,false);
    header.UUIDofSNAP = getStreamData(16, 408, input,"",true,false,false);

    globalHeader.offsetDataSize = header.offsetDataSize;
    globalHeader.blockSize = header.blockSize;
    globalHeader.blocksInHDD = header.blocksInHDD;

    map = new VdiMap (this, header.offsetBlocksSize, header.offsetBlocksSize + 4 *header.blocksInHDD, input);
    globalMap = map;
    mbr = new mbrData (this, header.offsetDataSize, input);


    unsigned int mbrDataTable0= mbr->getMbrDataTable(0);
    //nothing in the boot block, because this is not bootable
    bootBlockLocation = header.offsetDataSize+mbrDataTable0*512;
    superBlockLocation = bootBlockLocation +1024;

    superBlock = new ext2SuperBlock (this, superBlockLocation, input);

    unsigned int group_count = superBlock->getGroupCount();
    block_size = superBlock->getBlockSize();
    unsigned int inodes_per_group = superBlock->getInodesPerGroup();

    cout << "andy block_size" << block_size << endl;
    cout << "andy super block size" << superBlock->getBlockSize() << endl;
    cout << "andy blocksupergroup" << superBlock->getBlocksPerGroup() << endl;
    group_size = block_size*superBlock->getBlocksPerGroup();
    cout << "The total size of this block group is (bytes): " << group_size << endl;


    //cout << superBlock->getBlockSize() << "    " << superBlock->getGroupDescriptorBlockNumber()<< endl;
    unsigned int gdtLocation = bootBlockLocation + (block_size*superBlock->getGroupDescriptorBlockNumber());

    cout << "GDT Location(dec): " << hex << setw(2) << setfill('0') << gdtLocation << endl;

    cout << "Group count= " << dec <<group_count << endl;
    cout << "Block Size = " << dec << block_size << endl;



    //groupDescriptors->push_back(new ext2GroupDescriptor (this, group_count, gdtLocation, input));
    groupDescriptors = new ext2GroupDescriptor (this, group_count, gdtLocation, input);

    unsigned int block_bitmap_address = bootBlockLocation + block_size*groupDescriptors->getBlockBitmap(0);
    unsigned int inode_bitmap_address = bootBlockLocation + block_size*groupDescriptors->getInodeBitmap(0);
    cout << "Hello:: Inode Bitmap Location: " << hex << setw(2) << setfill('0') << inode_bitmap_address << endl;
    unsigned int inode_table_address = bootBlockLocation + block_size*groupDescriptors->getInodeTable(0);
    cout << "Hello:: Inode Table Location: " << hex << setw(2) << setfill('0') << inode_table_address << endl;

    //cout << hex << setw(2) << setfill('0') << inode_table_address <<" "<<endl << endl<< endl;
    //unsigned int block_bitmap = getStreamData(4,inode_bitmap_address, input);



    //DataBlockBitmap = new QVector <unsigned char>;
    // fillDataBlockBitmap(DataBlockBitmap, block_bitmap_address, inode_bitmap_address, input);
    //cout << "This is the adress of block bitmap" << hex<< block_bitmap_address << endl;

    addBitsFromStreamData(blockBitmap, block_size*8, block_bitmap_address, input);
    addBitsFromStreamData(inodesBitmap, block_size*8, inode_bitmap_address, input);
    //cout << dec <<blockBitmap->size() << " " << sizeof(*blockBitmap) << endl;
    //cout << blockBitmap->max_size() << endl;
    cout << "Bit reading/ converting complete" << endl;

    fsManager = new ext2FileSystemManager(&input, groupDescriptors, superBlock, bootBlockLocation, block_size);
    emit FSManagerConstructed(fsManager);

    transferToLocalFS(0,0);

}

void VdiFile::closeAndReset() {
    input.close();
    DataBlockBitmap->clear();
    blockBitmap->clear();
    inodesBitmap->clear();
}


void VdiFile::fillDataBlockBitmap(QVector<unsigned char>* DataBlockBitmap, unsigned int block_bitmap_address,unsigned int inode_bitmap_address,ifstream& input) {
    for (unsigned int i=block_bitmap_address; i <inode_bitmap_address; i++){
         DataBlockBitmap->push_back(getCharFromStream(1,i,input));
    }   
}

void VdiFile::transferToLocalFS(QString sourcePath, QString destPath) {

        OutputFileIntoLocalFS.open( "C:\\Users\\Andy\\Desktop\\TrialFor1K.txt", ios::out);
        if(OutputFileIntoLocalFS.is_open()){
            cout << "File is open for writing..." << endl;
        }

    /*
        //QDir myDirectory("");
       // myDirectory.setCurrent;


        // USE THIS TO REPLACE THE THINGS IN OUR PATH!!
        //mystring.replace("/","\\\\");
        QFile localFile("C:/Users/Andy/Desktop/sf-book.txt");
        if (!localFile.open(QIODevice::WriteOnly)) {
            std::cerr << "Cannot open file for writing: "
                      << qPrintable(localFile.errorString()) << std::endl;
            return;
        }

        QTextStream out(&localFile);
        //out << "Thomas M. Disch: " << 334 << endl;
*/
        loadLocalFile(33867,73, input, OutputFileIntoLocalFS);
        cout << "We got here" << endl;
}

void VdiFile::transferToVDI(QString sourcePath, QString destPath) {

}

void VdiFile::loadLocalFile(int inodeIndexNum, long long size, ifstream& input , ofstream& localFile){

    //Get Iblock(inodeIndexNum)
    int block_num = inodeIndexNum; //I just did this for now
    unsigned long long offset = bootBlockLocation+(block_size * (block_num));
    if((size - block_size) >0){
        for(int i=0; i<block_size; i++){
            input.seekg(offset+i);
            localFile << (char)input.get();
            input.clear();
            cout << "Looping in here: " << i << endl;
            }
        size=size-block_size;
        }
    else{
        for (int i=0; i<size; i++){
            input.seekg(offset+i);
            localFile << (char) input.get();
            input.clear();
        }
        size =0;
    }

    inodeIndexNum++;

    if(size!=0){
            loadLocalFile(inodeIndexNum, size, input, localFile);
            cout << "We got into here" << endl;
        }




cout << "hello" << endl;

}
