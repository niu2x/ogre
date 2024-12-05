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
#include "OgreInstanceBatchShader.h"
#include "OgreHardwareBufferManager.h"
#include "OgreInstanceManager.h"
#include "OgreInstancedEntity.h"
#include "OgreLogManager.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreMeshManager.h"
#include "OgreRenderOperation.h"
#include "OgreSubMesh.h"
#include "OgreTechnique.h"

namespace Ogre
{
    InstanceBatchShader::InstanceBatchShader( InstanceManager *creator, MeshPtr &meshReference,
                                        const MaterialPtr &material, size_t instancesPerBatch,
                                        const Mesh::IndexMap *indexToBoneMap, const String &batchName ) :
                InstanceBatch( creator, meshReference, material, instancesPerBatch,
                                indexToBoneMap, batchName ),
                mNumWorldMatrices( instancesPerBatch )
    {
    }

    //-----------------------------------------------------------------------
    size_t InstanceBatchShader::calculateMaxNumInstances( const SubMesh *baseSubMesh, uint16 flags ) const
    {
        const size_t numBones = std::max<size_t>( 1, baseSubMesh->blendIndexToBoneIndexMap.size() );

        mMaterial->load();
        Technique *technique = mMaterial->getBestTechnique();
        if( technique )
        {
            GpuProgramParametersSharedPtr vertexParam = technique->getPass(0)->getVertexProgramParameters();
            for(auto& it : vertexParam->getConstantDefinitions().map)
            {
                const GpuConstantDefinition &constDef = it.second;
                if(((constDef.constType == GCT_MATRIX_3X4 ||
                    constDef.constType == GCT_MATRIX_4X3 ||             //OGL GLSL bitches without this
                    constDef.constType == GCT_MATRIX_2X4 ||
                    constDef.constType == GCT_FLOAT4)                   //OGL GLSL bitches without this
                    && constDef.isFloat()) ||
                   ((constDef.constType == GCT_MATRIX_DOUBLE_3X4 ||
                    constDef.constType == GCT_MATRIX_DOUBLE_4X3 ||      //OGL GLSL bitches without this
                    constDef.constType == GCT_MATRIX_DOUBLE_2X4 ||
                    constDef.constType == GCT_DOUBLE4)                  //OGL GLSL bitches without this
                    && constDef.isDouble())
                   )
                {
                    const GpuProgramParameters::AutoConstantEntry *entry =
                                    vertexParam->_findRawAutoConstantEntryFloat( constDef.physicalIndex );
                    if( entry && (entry->paramType == GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY_3x4 || entry->paramType == GpuProgramParameters::ACT_WORLD_DUALQUATERNION_ARRAY_2x4))
                    {
                        //Material is correctly done!
                        size_t arraySize = constDef.arraySize;

                        //Deal with GL "hacky" way of doing 4x3 matrices
                        if(entry->paramType == GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY_3x4 && constDef.constType == GCT_FLOAT4)
                            arraySize /= 3;
                        else if(entry->paramType == GpuProgramParameters::ACT_WORLD_DUALQUATERNION_ARRAY_2x4 && constDef.constType == GCT_FLOAT4)
                            arraySize /= 2;

                        //Check the num of arrays
                        size_t retVal = arraySize / numBones;

                        if( flags & IM_USE16BIT )
                        {
                            if( baseSubMesh->vertexData->vertexCount * retVal > 0xFFFF )
                                retVal = 0xFFFF / baseSubMesh->vertexData->vertexCount;
                        }

                        if((retVal < 3 && entry->paramType == GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY_3x4) ||
                            (retVal < 2 && entry->paramType == GpuProgramParameters::ACT_WORLD_DUALQUATERNION_ARRAY_2x4))
                        {
                            LogManager::getSingleton().logWarning( "InstanceBatchShader: Mesh '" +
                                        mMeshReference->getName() + "' using material '" +
                                        mMaterial->getName() + "'. The amount of possible "
                                        "instances per batch is very low. Performance benefits will "
                                        "be minimal, if any. It might be even slower!");
                        }

                        return retVal;
                    }
                }
            }

            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Material '" + mMaterial->getName() + "' does not support hardware skinning");
        }

        //Reaching here the material is just unsupported.

        return 0;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::buildFrom( const SubMesh *baseSubMesh, const RenderOperation &renderOperation )
    {
        if( mMeshReference->hasSkeleton() && mMeshReference->getSkeleton() )
            mNumWorldMatrices = mInstancesPerBatch * baseSubMesh->blendIndexToBoneIndexMap.size();
        InstanceBatch::buildFrom( baseSubMesh, renderOperation );
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::setupVertices( const SubMesh* baseSubMesh )
    {
        mRenderOperation.vertexData = OGRE_NEW VertexData();
        mRemoveOwnVertexData = true; //Raise flag to remove our own vertex data in the end (not always needed)

        VertexData *thisVertexData = mRenderOperation.vertexData;
        VertexData *baseVertexData = baseSubMesh->vertexData;

        thisVertexData->vertexStart = 0;
        thisVertexData->vertexCount = baseVertexData->vertexCount * mInstancesPerBatch;

        HardwareBufferManager::getSingleton().destroyVertexDeclaration( thisVertexData->vertexDeclaration );
        thisVertexData->vertexDeclaration = baseVertexData->vertexDeclaration->clone();

        if( mMeshReference->hasSkeleton() && mMeshReference->getSkeleton() )
        {
            //Building hw skinned batches follow a different path
            setupHardwareSkinned( baseSubMesh, thisVertexData, baseVertexData );
            return;
        }

        //TODO: Can't we, instead of using another source, put the index ID in the same source?
        thisVertexData->vertexDeclaration->addElement(
                                        thisVertexData->vertexDeclaration->getMaxSource() + 1, 0,
                                        VET_UBYTE4, VES_BLEND_INDICES );


        for( uint16 i=0; i<thisVertexData->vertexDeclaration->getMaxSource(); ++i )
        {
            //Create our own vertex buffer
            HardwareVertexBufferSharedPtr vertexBuffer =
                                            HardwareBufferManager::getSingleton().createVertexBuffer(
                                            thisVertexData->vertexDeclaration->getVertexSize(i),
                                            thisVertexData->vertexCount,
                                            HardwareBuffer::HBU_STATIC_WRITE_ONLY );
            thisVertexData->vertexBufferBinding->setBinding( i, vertexBuffer );

            //Grab the base submesh data
            HardwareVertexBufferSharedPtr baseVertexBuffer =
                                                    baseVertexData->vertexBufferBinding->getBuffer(i);

            HardwareBufferLockGuard thisLock(vertexBuffer, HardwareBuffer::HBL_DISCARD);
            HardwareBufferLockGuard baseLock(baseVertexBuffer, HardwareBuffer::HBL_READ_ONLY);
            char* thisBuf = static_cast<char*>(thisLock.pData);
            char* baseBuf = static_cast<char*>(baseLock.pData);

            //Copy and repeat
            for( size_t j=0; j<mInstancesPerBatch; ++j )
            {
                const size_t sizeOfBuffer = baseVertexData->vertexCount *
                                            baseVertexData->vertexDeclaration->getVertexSize(i);
                memcpy( thisBuf + j * sizeOfBuffer, baseBuf, sizeOfBuffer );
            }
        }

        {
            //Now create the vertices "index ID" to individualize each instance
            const unsigned short lastSource = thisVertexData->vertexDeclaration->getMaxSource();
            HardwareVertexBufferSharedPtr vertexBuffer =
                                            HardwareBufferManager::getSingleton().createVertexBuffer(
                                            thisVertexData->vertexDeclaration->getVertexSize( lastSource ),
                                            thisVertexData->vertexCount,
                                            HardwareBuffer::HBU_STATIC_WRITE_ONLY );
            thisVertexData->vertexBufferBinding->setBinding( lastSource, vertexBuffer );

            HardwareBufferLockGuard thisLock(vertexBuffer, HardwareBuffer::HBL_DISCARD);
            char* thisBuf = static_cast<char*>(thisLock.pData);
            for( uint8 j=0; j<uint8(mInstancesPerBatch); ++j )
            {
                for( size_t k=0; k<baseVertexData->vertexCount; ++k )
                {
                    *thisBuf++ = j;
                    *thisBuf++ = j;
                    *thisBuf++ = j;
                    *thisBuf++ = j;
                }
            }

        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::setupIndices( const SubMesh* baseSubMesh )
    {
        mRenderOperation.indexData = OGRE_NEW IndexData();
        mRemoveOwnIndexData = true; //Raise flag to remove our own index data in the end (not always needed)

        IndexData *thisIndexData = mRenderOperation.indexData;
        IndexData *baseIndexData = baseSubMesh->indexData;

        thisIndexData->indexStart = 0;
        thisIndexData->indexCount = baseIndexData->indexCount * mInstancesPerBatch;

        //TODO: Check numVertices is below max supported by GPU
        HardwareIndexBuffer::IndexType indexType = HardwareIndexBuffer::IT_16BIT;
        if( mRenderOperation.vertexData->vertexCount > 65535 )
            indexType = HardwareIndexBuffer::IT_32BIT;
        thisIndexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
            indexType, thisIndexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY );

        HardwareBufferLockGuard thisLock(thisIndexData->indexBuffer, HardwareBuffer::HBL_DISCARD);
        HardwareBufferLockGuard baseLock(baseIndexData->indexBuffer, HardwareBuffer::HBL_READ_ONLY);
        uint16 *thisBuf16 = static_cast<uint16*>(thisLock.pData);
        uint32 *thisBuf32 = static_cast<uint32*>(thisLock.pData);
        bool baseIndex16bit = baseIndexData->indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT;

        for( size_t i=0; i<mInstancesPerBatch; ++i )
        {
            const size_t vertexOffset = i * mRenderOperation.vertexData->vertexCount / mInstancesPerBatch;

            const uint16 *initBuf16 = static_cast<const uint16 *>(baseLock.pData);
            const uint32 *initBuf32 = static_cast<const uint32 *>(baseLock.pData);

            for( size_t j=0; j<baseIndexData->indexCount; ++j )
            {
                uint32 originalVal = baseIndex16bit ? *initBuf16++ : *initBuf32++;

                if( indexType == HardwareIndexBuffer::IT_16BIT )
                    *thisBuf16++ = static_cast<uint16>(originalVal + vertexOffset);
                else
                    *thisBuf32++ = static_cast<uint32>(originalVal + vertexOffset);
            }
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::setupHardwareSkinned( const SubMesh* baseSubMesh, VertexData *thisVertexData,
                                                    VertexData *baseVertexData )
    {
        const uint8 numBones = uint8(baseSubMesh->blendIndexToBoneIndexMap.size());
        mNumWorldMatrices = mInstancesPerBatch * numBones;

        for( uint16 i=0; i<=thisVertexData->vertexDeclaration->getMaxSource(); ++i )
        {
            //Create our own vertex buffer
            HardwareVertexBufferSharedPtr vertexBuffer =
                                            HardwareBufferManager::getSingleton().createVertexBuffer(
                                            thisVertexData->vertexDeclaration->getVertexSize(i),
                                            thisVertexData->vertexCount,
                                            HardwareBuffer::HBU_STATIC_WRITE_ONLY );
            thisVertexData->vertexBufferBinding->setBinding( i, vertexBuffer );

            VertexDeclaration::VertexElementList veList =
                                            thisVertexData->vertexDeclaration->findElementsBySource(i);

            //Grab the base submesh data
            HardwareVertexBufferSharedPtr baseVertexBuffer =
                                                    baseVertexData->vertexBufferBinding->getBuffer(i);

            HardwareBufferLockGuard thisVertexLock(vertexBuffer, HardwareBuffer::HBL_DISCARD);
            HardwareBufferLockGuard baseVertexLock(baseVertexBuffer, HardwareBuffer::HBL_READ_ONLY);
            char* thisBuf = static_cast<char*>(thisVertexLock.pData);
            char* baseBuf = static_cast<char*>(baseVertexLock.pData);
            char *startBuf = baseBuf;

            //Copy and repeat
            for (uint8 j = 0; j < uint8(mInstancesPerBatch); ++j)
            {
                //Repeat source
                baseBuf = startBuf;

                for (size_t k = 0; k < baseVertexData->vertexCount; ++k)
                {
                    for (auto& vl : veList)
                    {
                        switch (vl.getSemantic())
                        {
                        case VES_BLEND_INDICES:
                        *(thisBuf + vl.getOffset() + 0) = *(baseBuf + vl.getOffset() + 0) + j * numBones;
                        *(thisBuf + vl.getOffset() + 1) = *(baseBuf + vl.getOffset() + 1) + j * numBones;
                        *(thisBuf + vl.getOffset() + 2) = *(baseBuf + vl.getOffset() + 2) + j * numBones;
                        *(thisBuf + vl.getOffset() + 3) = *(baseBuf + vl.getOffset() + 3) + j * numBones;
                            break;
                        default:
                            memcpy( thisBuf + vl.getOffset(), baseBuf + vl.getOffset(), vl.getSize() );
                            break;
                        }
                    }
                    thisBuf += baseVertexData->vertexDeclaration->getVertexSize(i);
                    baseBuf += baseVertexData->vertexDeclaration->getVertexSize(i);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::getWorldTransforms( Matrix4* xform ) const
    {
        if (MeshManager::getBonesUseObjectSpace())
        {
            *xform = Affine3::IDENTITY;
            xform++;
        }

        for (auto *e : mInstancedEntities)
        {
            xform += e->getTransforms(xform);
        }
    }
    //-----------------------------------------------------------------------
    unsigned short InstanceBatchShader::getNumWorldTransforms(void) const
    {
        return uint16(mNumWorldMatrices) + uint16(MeshManager::getBonesUseObjectSpace());
    }
}
