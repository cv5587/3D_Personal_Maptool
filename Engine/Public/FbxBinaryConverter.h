#pragma once
#include "Base.h"
class CFbxBinaryConverter final:
    public CBase
{
    typedef struct{
        _uint NumMesh;
           
               wstring MeshName;//�޽��̸�
               _uint    MaterialIdx;//���׸���
               _uint    Numbones;//�����ִ� ������
               _uint* m_BoneIdxces;//������ �ε���
               _float4x4* OffsetMat;//���� ���� ������
               _uint    PrimitiveTopology;
               _uint    IndexFormat;
               _uint    NumVertexBuffers;

               _uint    NumVertices;
               _uint    VertexStride;
               D3D11_BUFFER_DESC VBuffer_Desc;
               VTXMESH*  VerticesInfo;

               _uint    NumIndeces;
               _uint    IndexStride;
               D3D11_BUFFER_DESC IBuffer_Desc;
               _uint*      Indices;


        _uint NumMaterial;
            //���׸��� �ϳ��� 21���� ���� �ʿ� (1���׸����� 
            _uint*   NumTexture;
            wstring  TextureFilePath;

        _uint NumBone;
                //bone ������ŭ
              wstring szName;
              _int*    ParentBoneIdx;
              _float4x4*   TransformMat;


    }NONANIM_DATA;

private:
    CFbxBinaryConverter();
    virtual ~CFbxBinaryConverter() = default;

public:
    HRESULT Initialize();
    HRESULT Save_Binary(void* pArg);
    HRESULT Load_Binary();
public:
    static CFbxBinaryConverter* Create();
    virtual void Free()override;
};

