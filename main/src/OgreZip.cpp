/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "OgreStableHeaders.h"

#if OGRE_NO_ZIP_ARCHIVE == 0
#include <zip.h>

namespace Ogre {
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //  EmbeddedZipArchiveFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    /// a struct to hold embedded file data
    struct EmbeddedFileData
    {
        const uint8 * fileData;
        size_t fileSize;
        size_t curPos;
        bool isFileOpened;
        EmbeddedZipArchiveFactory::DecryptEmbeddedZipFileFunc decryptFunc;
    };
    //-----------------------------------------------------------------------
    /// A type for a map between the file names to file index
    typedef std::map<String, EmbeddedFileData> EmbbedFileDataList;

    namespace {
    /// A static list to store the embedded files data
    EmbbedFileDataList *gEmbeddedFileDataList;
    } // namespace {
    //-----------------------------------------------------------------------
    EmbeddedZipArchiveFactory::EmbeddedZipArchiveFactory() {}
    EmbeddedZipArchiveFactory::~EmbeddedZipArchiveFactory() {}
    //-----------------------------------------------------------------------
    Archive *EmbeddedZipArchiveFactory::createInstance( const String& name, bool readOnly )
    {
        auto it = gEmbeddedFileDataList->find(name);
        if(it == gEmbeddedFileDataList->end())
            return NULL;

        // TODO: decryptFunc

        return new ZipArchive(name, getType(), it->second.fileData, it->second.fileSize);
    }
    void EmbeddedZipArchiveFactory::destroyInstance(Archive* ptr)
    {
        removeEmbbeddedFile(ptr->getName());
        ZipArchiveFactory::destroyInstance(ptr);
    }
    //-----------------------------------------------------------------------
    const String& EmbeddedZipArchiveFactory::getType(void) const
    {
        static String name = "EmbeddedZip";
        return name;
    }
    //-----------------------------------------------------------------------
    void EmbeddedZipArchiveFactory::addEmbbeddedFile(const String& name, const uint8 * fileData, 
                                        size_t fileSize, DecryptEmbeddedZipFileFunc decryptFunc)
    {
        static bool needToInit = true;
        if(needToInit)
        {
            needToInit = false;

            // we can't be sure when global variables get initialized
            // meaning it is possible our list has not been init when this
            // function is being called. The solution is to use local
            // static members in this function an init the pointers for the
            // global here. We know for use that the local static variables
            // are create in this stage.
            static EmbbedFileDataList sEmbbedFileDataList;
            gEmbeddedFileDataList = &sEmbbedFileDataList;
        }

        EmbeddedFileData newEmbeddedFileData;
        newEmbeddedFileData.curPos = 0;
        newEmbeddedFileData.isFileOpened = false;
        newEmbeddedFileData.fileData = fileData;
        newEmbeddedFileData.fileSize = fileSize;
        newEmbeddedFileData.decryptFunc = decryptFunc;
        gEmbeddedFileDataList->emplace(name, newEmbeddedFileData);
    }
    //-----------------------------------------------------------------------
    void EmbeddedZipArchiveFactory::removeEmbbeddedFile( const String& name )
    {
        gEmbeddedFileDataList->erase(name);
    }
}

#endif
