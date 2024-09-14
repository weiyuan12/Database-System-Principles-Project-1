#include <fstream>
#include <iostream>
#include "./BPTree.cpp"
#include "./DataFile.cpp"

int main()
{
    std::fstream indexFile("index.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    std::fstream entriesFile("entries.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    if (!indexFile.is_open() || !entriesFile.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    Storage indexStorage = Storage(&indexFile);
    Storage entriesStorage = Storage(&entriesFile);

    BPTree bptree = BPTree(&indexStorage);
    DataFile dataFile = DataFile(&entriesStorage);

    // TODO: add bulkLoad
    if (false) // TO_BULKLOAD
    {
        // bptree.bulkLoad(&dataStorage);
    }

    // TODO: implement find
    int key = bptree.find(1);
}
