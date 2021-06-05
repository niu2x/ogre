// This file is part of the OGRE project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at https://www.ogre3d.org/licensing.
// SPDX-License-Identifier: MIT

#include "OgreGLSLangProgramManager.h"
#include "OgreRoot.h"

#include "OgreLogManager.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/MachineIndependent/gl_types.h>

namespace
{
const TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    /* .maxDualSourceDrawBuffersEXT =  1,*/

    /* .limits = */
    {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }};
}

namespace Ogre
{
/// Convert GL uniform size and type to OGRE constant types
static GpuConstantType mapToGCT(int gltype)
{
    switch (gltype)
    {
    case GL_FLOAT:
        return GCT_FLOAT1;
    case GL_FLOAT_VEC2:
        return GCT_FLOAT2;
    case GL_FLOAT_VEC3:
        return GCT_FLOAT3;
    case GL_FLOAT_VEC4:
        return GCT_FLOAT4;
    case GL_IMAGE_1D: //TODO should be its own type?
    case GL_SAMPLER_1D:
    case GL_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
        // need to record samplers for GLSL
        return GCT_SAMPLER1D;
    case GL_IMAGE_2D: //TODO should be its own type?
    case GL_IMAGE_2D_RECT:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_2D_RECT:    // TODO: Move these to a new type??
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
        return GCT_SAMPLER2D;
    case GL_IMAGE_3D: //TODO should be its own type?
    case GL_SAMPLER_3D:
    case GL_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
        return GCT_SAMPLER3D;
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
        return GCT_SAMPLERCUBE;
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
        return GCT_SAMPLER1DSHADOW;
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
        return GCT_SAMPLER2DSHADOW;
    case GL_INT:
        return GCT_INT1;
    case GL_INT_VEC2:
        return GCT_INT2;
    case GL_INT_VEC3:
        return GCT_INT3;
    case GL_INT_VEC4:
        return GCT_INT4;
    case GL_FLOAT_MAT2:
        return GCT_MATRIX_2X2;
    case GL_FLOAT_MAT3:
        return GCT_MATRIX_3X3;
    case GL_FLOAT_MAT4:
        return GCT_MATRIX_4X4;
    case GL_FLOAT_MAT2x3:
        return GCT_MATRIX_2X3;
    case GL_FLOAT_MAT3x2:
        return GCT_MATRIX_3X2;
    case GL_FLOAT_MAT2x4:
        return GCT_MATRIX_2X4;
    case GL_FLOAT_MAT4x2:
        return GCT_MATRIX_4X2;
    case GL_FLOAT_MAT3x4:
        return GCT_MATRIX_3X4;
    case GL_FLOAT_MAT4x3:
        return GCT_MATRIX_4X3;
    case GL_DOUBLE:
        return GCT_DOUBLE1;
    case GL_DOUBLE_VEC2:
        return GCT_DOUBLE2;
    case GL_DOUBLE_VEC3:
        return GCT_DOUBLE3;
    case GL_DOUBLE_VEC4:
        return GCT_DOUBLE4;
    case GL_DOUBLE_MAT2:
        return GCT_MATRIX_DOUBLE_2X2;
    case GL_DOUBLE_MAT3:
        return GCT_MATRIX_DOUBLE_3X3;
    case GL_DOUBLE_MAT4:
        return GCT_MATRIX_DOUBLE_4X4;
    case GL_DOUBLE_MAT2x3:
        return GCT_MATRIX_DOUBLE_2X3;
    case GL_DOUBLE_MAT3x2:
        return GCT_MATRIX_DOUBLE_3X2;
    case GL_DOUBLE_MAT2x4:
        return GCT_MATRIX_DOUBLE_2X4;
    case GL_DOUBLE_MAT4x2:
        return GCT_MATRIX_DOUBLE_4X2;
    case GL_DOUBLE_MAT3x4:
        return GCT_MATRIX_DOUBLE_3X4;
    case GL_DOUBLE_MAT4x3:
        return GCT_MATRIX_DOUBLE_4X3;
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_ATOMIC_COUNTER: //TODO should be its own type?
        return GCT_UINT1;
    case GL_UNSIGNED_INT_VEC2:
        return GCT_UINT2;
    case GL_UNSIGNED_INT_VEC3:
        return GCT_UINT3;
    case GL_UNSIGNED_INT_VEC4:
        return GCT_UINT4;
    case GL_BOOL:
        return GCT_BOOL1;
    case GL_BOOL_VEC2:
        return GCT_BOOL2;
    case GL_BOOL_VEC3:
        return GCT_BOOL3;
    case GL_BOOL_VEC4:
        return GCT_BOOL4;
    default:
        return GCT_UNKNOWN;
    }
}

//-----------------------------------------------------------------------
static String sLanguageName = "glslang";

static EShLanguage getShLanguage(GpuProgramType type)
{
    switch (type)
    {
    // clang-format off
    case GPT_VERTEX_PROGRAM:    return EShLangVertex;
    case GPT_FRAGMENT_PROGRAM:  return EShLangFragment;
    case GPT_GEOMETRY_PROGRAM:  return EShLangGeometry;
    case GPT_HULL_PROGRAM:      return EShLangTessControl;
    case GPT_DOMAIN_PROGRAM:    return EShLangTessEvaluation;
    case GPT_COMPUTE_PROGRAM:   return EShLangCompute;
        // clang-format on
    }

    return EShLangFragment;
}

GLSLangProgram::GLSLangProgram(ResourceManager* creator, const String& name, ResourceHandle handle,
                               const String& group, bool isManual, ManualResourceLoader* loader)
    : HighLevelGpuProgram(creator, name, handle, group, isManual, loader)
{
}

const String& GLSLangProgram::getLanguage(void) const
{
    return sLanguageName;
}

void GLSLangProgram::createLowLevelImpl()
{
    if (mCompileError)
        return;

    mAssemblerProgram =
        GpuProgramManager::getSingleton().createProgram(mName + "/Delegate", mGroup, "gl_spirv", mType);
    String assemblyStr((char*)mAssembly.data(), mAssembly.size() * sizeof(uint32));
    mAssemblerProgram->setSource(assemblyStr);
    mAssembly.clear(); // delegate stores the data now
}
void GLSLangProgram::unloadHighLevelImpl() {}

void GLSLangProgram::prepareImpl()
{
    HighLevelGpuProgram::prepareImpl();

    auto stage = getShLanguage(mType);
    glslang::TShader shader(stage);
    const char* source = mSource.c_str();
    const char* name = mFilename.empty() ? NULL : mFilename.c_str();

    shader.setStringsWithLengthsAndNames(&source, NULL, &name, 1);
    shader.setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);

    // require 430 for explicit uniform location
    if (!shader.parse(&DefaultTBuiltInResource, 430, false, EShMsgSpvRules))
    {
        LogManager::getSingleton().logError("GLSLang compilation failed for " + mName + ":\n" +
                                            shader.getInfoLog());
        mCompileError = true;
        return;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgSpvRules))
    {
        LogManager::getSingleton().logError("GLSLang linking failed for " + mName + ":\n" +
                                            program.getInfoLog());
        mCompileError = true;
        return;
    }

    auto intermediate = program.getIntermediate(stage);
    glslang::SpvOptions opts;
    glslang::GlslangToSpv(*intermediate, mAssembly, &opts);

    getConstantDefinitions();
    program.buildReflection();

    int nuniforms = program.getNumUniformVariables();
    for(int i = 0; i < nuniforms; i++)
    {
        auto u = program.getUniform(i);
        if(u.getType()->isOpaque())
            continue;

        GpuConstantDefinition def;
        def.logicalIndex = u.getType()->getQualifier().layoutLocation;
        def.arraySize = u.size;
        def.physicalIndex = mConstantDefs->bufferSize * 4;
        def.constType = mapToGCT(u.glDefineType);
        def.elementSize = GpuConstantDefinition::getElementSize(def.constType, false);

        mConstantDefs->bufferSize += def.arraySize * def.elementSize;
        mConstantDefs->map.emplace(u.name, def);

        // also allow index based referencing
        GpuLogicalIndexUse use;
        use.physicalIndex = def.physicalIndex;
        use.currentSize = def.arraySize * def.elementSize;
        mLogicalToPhysical->map.emplace(def.logicalIndex, use);
    }
}

void GLSLangProgram::loadFromSource() {}
void GLSLangProgram::buildConstantDefinitions() {}

//-----------------------------------------------------------------------
GLSLangProgramFactory::GLSLangProgramFactory()
{
    glslang::InitializeProcess();
}
//-----------------------------------------------------------------------
GLSLangProgramFactory::~GLSLangProgramFactory()
{
    glslang::FinalizeProcess();
}
//-----------------------------------------------------------------------
const String& GLSLangProgramFactory::getLanguage(void) const
{
    return sLanguageName;
}
//-----------------------------------------------------------------------
GpuProgram* GLSLangProgramFactory::create(ResourceManager* creator, const String& name,
                                          ResourceHandle handle, const String& group, bool isManual,
                                          ManualResourceLoader* loader)
{
    return OGRE_NEW GLSLangProgram(creator, name, handle, group, isManual, loader);
}
const String& GLSLangPlugin::getName() const
{
    static String sPluginName = "GLSLang Program Manager";
    return sPluginName;
}
//---------------------------------------------------------------------
void GLSLangPlugin::install() {}
//---------------------------------------------------------------------
void GLSLangPlugin::initialise()
{
    // Create new factory
    mProgramFactory.reset(new GLSLangProgramFactory());

    // Register
    GpuProgramManager::getSingleton().addFactory(mProgramFactory.get());
}
//---------------------------------------------------------------------
void GLSLangPlugin::shutdown()
{
    // nothing to do
}
//---------------------------------------------------------------------
void GLSLangPlugin::uninstall()
{
    if (mProgramFactory)
    {

        // Remove from manager safely
        if (GpuProgramManager::getSingletonPtr())
            GpuProgramManager::getSingleton().removeFactory(mProgramFactory.get());
        mProgramFactory.reset();
    }
}

#ifndef OGRE_STATIC_LIB
extern "C" void _OgreGLSLangProgramManagerExport dllStartPlugin(void);
extern "C" void _OgreGLSLangProgramManagerExport dllStopPlugin(void);

static Plugin* glslangPlugin;
//-----------------------------------------------------------------------
extern "C" void _OgreGLSLangProgramManagerExport dllStartPlugin(void)
{
    // Create new plugin
    glslangPlugin = OGRE_NEW GLSLangPlugin();

    // Register
    Root::getSingleton().installPlugin(glslangPlugin);
}
extern "C" void _OgreGLSLangProgramManagerExport dllStopPlugin(void)
{
    Root::getSingleton().uninstallPlugin(glslangPlugin);
    OGRE_DELETE glslangPlugin;
}
#endif

} // namespace Ogre
