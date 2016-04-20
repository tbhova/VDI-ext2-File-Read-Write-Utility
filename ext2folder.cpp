#include "ext2folder.h"

using namespace std;
using namespace CSCI5806;

ext2Folder::ext2Folder(InodeTable tab, QString entryName) : ext2FSEntry(tab, entryName)
{
    folders = new QVector<ext2Folder*>;
    files = new QVector<ext2File*>;
}

QVector<ext2Folder*>* ext2Folder::getFolders() {
    return folders;
}

QVector<ext2File*>* ext2Folder::getFiles() {
    return files;
}