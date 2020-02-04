#pragma once 

#include "AIRStaticMesh.h"
#include "AirMeshDef.h"

class CAIRStaticMeshFile
{
public:
    CAIRStaticMeshFile() : m_nVersion(1), m_strCoord(STR_COORDINATE_RIGHT), m_strUpVec(STR_AXIS_Z) { };
    ~CAIRStaticMeshFile() {};

    void SetVersion(int nVersion)
    {
        m_nVersion = nVersion;
    }

    int GetVersion() { return m_nVersion; }

    void AddMesh(const CAIRStaticMesh& mesh)
    {
        m_vtMeshes.push_back(mesh);
    }

    bool ReadFromFile(const char* szFilename)
    {
        FILE* fp = fopen(szFilename, "r");

        if (fp == NULL)
        {
            return false;
        }

        char buff[512];
        int version = 0;
        int numMeshes = 0;

        while (!feof (fp))
        {
            /* Read whole line */
            fgets (buff, sizeof (buff), fp);

            if (sscanf (buff, " AIRVersion %d", &version) == 1)
            {
                if (version != 1)
                {
                    /* Bad version */
                    //fprintf (stderr, "Error: bad model version\n");
                    fclose (fp);
                    return false;
                }
            }
            else if (sscanf (buff, " numMeshes %d", &numMeshes) == 1)
            {
                if (numMeshes > 0)
                {
                    m_vtMeshes.reserve(numMeshes);
                }
                //if (pModel->nNumJoints > 0)
                //{
                //    /* Allocate memory for base skeleton joints */
                //    pModel->pBaseSkeleton = (LPMD5JOINT)
                //        calloc (pModel->nNumJoints, sizeof (MD5JOINT));
                //}
            }
          
            /*
            else if (strncmp (buff, "mesh {", 6) == 0)
            {
                
                for (i = 0; i < pModel->nNumJoints; ++i)
                {
                    LPMD5JOINT pJoint = &pModel->pBaseSkeleton[i];

                   
                    fgets (buff, sizeof (buff), fp);

                    if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )",
                        pJoint->szName, &pJoint->nParent, &pJoint->vPos[0],
                        &pJoint->vPos[1], &pJoint->vPos[2], &pJoint->orient.x,
                        &pJoint->orient.y, &pJoint->orient.z) == 8)
                    {
                        
                        QuarComputeW(pJoint->orient);
                    }
                }
            }
            */
            else if (strncmp (buff, "mesh {", 6) == 0)
            {
                CAIRStaticMesh* pMesh = new CAIRStaticMesh();
                //LPMD5MESH pMesh = &pModel->pMeshes[curr_mesh];
                int vert_index = 0;
                int tri_index = 0;
                //int weight_index = 0;
                float fdata[4];
                float uv[2];
                int idata[3];

                int  nVertices = 0;
                int  nTriangles = 0;
                int  nVertexIndex = 0;
                int  weight_index = 0;

                while ((buff[0] != '}') && !feof (fp))
                {
                    /* Read whole line */
                    fgets (buff, sizeof (buff), fp);

                    char szTmp[512];
                    
                    
                    if (sscanf(buff, "name %s", szTmp))
                    {
                        pMesh->SetName(string(szTmp));
                    }
                    else if (strstr (buff, "texture "))
                    {
                        int quote = 0, j = 0;
                        memset(szTmp, 0, sizeof(szTmp));

                        /* Copy the shader name whithout the quote marks */
                        for (int i = 0; i < sizeof (buff) && (quote < 2); ++i)
                        {
                            if (buff[i] == '\"')
                                quote++;

                            if ((quote == 1) && (buff[i] != '\"'))
                            {
                                szTmp[j] = buff[i];
                                j++;
                            }
                        }

                        pMesh->SetTextureName(string(szTmp));
                    }
                    else if (sscanf (buff, " num_vertices %d", &nVertices) == 1)
                    {
                        if (nVertices > 0)
                        {
                            /* Allocate memory for vertices */
                            //pMesh->m_vtMeshVertices.resize(nVertices);
                            
                        }

                        //if (mesh->num_verts > max_verts)         //不明白有什么用？
                        //	max_verts = mesh->num_verts;
                    }
                    else if (sscanf (buff, " num_triangles %d", &nTriangles) == 1)
                    {
                        if (nTriangles > 0)
                        {
                            /* Allocate memory for triangles */
                            //pMesh->m_vtFaces.resize(nTriangles);
                        }

                        //if (mesh->num_tris > max_tris)
                        //	max_tris = mesh->num_tris;
                    }
                    else if (sscanf (buff, " numweights %d", &nVertices) == 1)
                    {

                    }
                    else if (sscanf (buff, " mesh_vertex %d (%f, %f, &f) (%f %f)", &vert_index,
                        &fdata[0], &fdata[1], &fdata[2], &uv[0], &uv[1]) == 5)
                    {
                        /* Copy vertex data */
                        /*
                        pMesh->m_vtMeshVertices[vert_index].nIndex = vert_index;
                        pMesh->m_vtMeshVertices[vert_index].x = fdata[0];
                        pMesh->m_vtMeshVertices[vert_index].y = fdata[1];
                        pMesh->m_vtMeshVertices[vert_index].z = fdata[2];
                        pMesh->m_vtMeshVertices[vert_index].tu = uv[0];
                        pMesh->m_vtMeshVertices[vert_index].tv = uv[1];
                        */
                    }
                    else if (sscanf (buff, " mesh_triangle %d (%d %d %d)", &tri_index,
                        &idata[0], &idata[1], &idata[2]) == 4)
                    {
                        /* Copy triangle data */
                        /*
                        pMesh->m_vtFaces[tri_index ].nIndex[0] = idata[0];
                        pMesh->m_vtFaces[tri_index ].nIndex[1] = idata[1];
                        pMesh->m_vtFaces[tri_index ].nIndex[2] = idata[2];
                        */
                    }
                    else if (sscanf (buff, " weight %d %d %f ( %f %f %f )",
                        &weight_index, &idata[0], &fdata[3],
                        &fdata[0], &fdata[1], &fdata[2]) == 6)
                    {
                        /* Copy vertex data */
                        /*
                        pMesh->pWeights[weight_index].nJoint  = idata[0];
                        pMesh->pWeights[weight_index].fBias   = fdata[3];
                        pMesh->pWeights[weight_index].vPos[0] = fdata[0];
                        pMesh->pWeights[weight_index].vPos[1] = fdata[1];
                        pMesh->pWeights[weight_index].vPos[2] = fdata[2];
                        */
                    }
                }

                //curr_mesh++;
            }
        }

        fclose(fp);
        return true;
    }

    bool SaveToFile(const char* szFilename)
    {
        FILE* f = fopen(szFilename, "wt");

        if (f == NULL)
        {
            return false;
        }

        fprintf(f, "AIRVersion %d\n", m_nVersion);

        fprintf(f, "Coordinate %s\n", m_strCoord.c_str());

        fprintf(f, "UpVec %s\n", m_strUpVec.c_str());

        fprintf(f, "%s %d\n", STR_NUMMESH, m_vtMeshes.size());

        for (int i = 0; i < m_vtMeshes.size(); i++)
        {
            WriteMeshToFile(f, m_vtMeshes[i]);
        }

        fclose(f);
        return true;
    }
protected:
private:
    int  m_nVersion;     //版本号
    string                 m_strCoord;   //坐标系
    string                 m_strUpVec;   //向上的坐标轴，一般是Z或者Y
    vector<CAIRStaticMesh> m_vtMeshes;


    void WriteMeshToFile(FILE* f, const CAIRStaticMesh& mesh)
    {
        /*
        fprintf(f, "%s {\n", STR_MESH_HEADER);

        //string& strMeshName = mesh.GetName();
        fprintf(f, "    name %s\n", mesh.GetName().c_str());

        fprintf(f, "    texture %s\n", mesh.GetTextureName().c_str());

        fprintf(f, "    %s %d\n", STR_NUM_VERTS, mesh.GetVerticesCount());

        for (int i = 0; i < mesh.GetVerticesCount(); i++)
        {
            const AIRMESHVERT& Vert = mesh.GetMeshVert(i);
            fprintf(f, "    %s %d (%f, %f, %f) (%f, %f)\n", STR_MESH_VERT, i, Vert.x, Vert.y, Vert.z,
                Vert.tu, Vert.tv);
        }

        fprintf(f, "    %s %d\n", STR_NUM_TRIANGLES, mesh.GetFaceCount());
        for (int i = 0; i < mesh.GetFaceCount(); i++)
        {
            const AIRMESHTRI& Tri = mesh.GetMeshFace(i);
            fprintf(f, "    %s %d (%d %d %d)\n", STR_MESH_TRIANGLE, i, Tri.nIndex[0], Tri.nIndex[1], Tri.nIndex[2]);
        }

        //const AIRMATERIAL& material = mesh.GetMaterial();
        fprintf(f, "    %s color %d ambient(%f, %f, %f) difusse(%f, %f, %f) specular(%f, %f, %f) shiness %f\n", STR_MATERIAL, 
            mesh.m_material.Ambient[0], mesh.m_material.Ambient[1], mesh.m_material.Ambient[2], 
            mesh.m_material.Diffuse[0], mesh.m_material.Diffuse[1], mesh.m_material.Diffuse[2],
            mesh.m_material.Specular[0], mesh.m_material.Specular[1], mesh.m_material.Specular[2],
            mesh.m_material.Shiness);

        //const AIRBOUNDINGBOX& boundingBox = mesh.GetBoundingBox();

        fprintf(f, "    %s max(%f, %f, %f) min(%f, %f, %f)\n", STR_MESH_BOUNDINGBOX, mesh.m_boundingBox.maxX, mesh.m_boundingBox.maxY, mesh.m_boundingBox.maxZ,
            mesh.m_boundingBox.minX, mesh.m_boundingBox.minY, mesh.m_boundingBox.minZ);
        //fprintf()
 
        fprintf(f, "}\n\n");
        */
    }
};
