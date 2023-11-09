#pragma once
#include "../zoo/procGen.h"

namespace zoo {
    struct Vertex {
        ew::Vec3 pos;
        ew::Vec3 normal;
        ew::Vec2 uv;
    };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    ew::MeshData createPlane(float width, float height, int subdivisions)
    {
        ew::MeshData meshData;

        int numVerticesX = subdivisions + 1;
        int numVerticesY = subdivisions + 1;
        float stepX = width / subdivisions;
        float stepY = height / subdivisions;

        for (int y = 0; y < numVerticesY; ++y) {
            for (int x = 0; x < numVerticesX; ++x) {
                ew::Vertex vertex;
                vertex.pos = { stepX * x - width * 0.5f, 0.0f, stepY * y - height * 0.5f };
                vertex.normal = { 0.0f, 1.0f, 0.0f };
                vertex.uv = { static_cast<float>(x) / subdivisions, static_cast<float>(y) / subdivisions };

                meshData.vertices.push_back(vertex);
            }
        }

        for (int y = 0; y < subdivisions; ++y) {
            for (int x = 0; x < subdivisions; ++x) {
                unsigned int i0 = y * numVerticesX + x;
                unsigned int i1 = y * numVerticesX + x + 1;
                unsigned int i2 = (y + 1) * numVerticesX + x;
                unsigned int i3 = (y + 1) * numVerticesX + x + 1;

                meshData.indices.push_back(i0);
                meshData.indices.push_back(i2);
                meshData.indices.push_back(i1);

                meshData.indices.push_back(i1);
                meshData.indices.push_back(i2);
                meshData.indices.push_back(i3);
            }
        }

        return meshData;

    }


    ew::MeshData createSphere(float radius, int numSegments)
    {
        ew::MeshData meshData;

        for (int i = 0; i <= numSegments; i++) {
            float phi = ((float)i / numSegments) * ew::PI;
            float y = radius * cos(phi);
            float r = radius * sin(phi);

            for (int j = 0; j <= numSegments; j++) {
                float theta = ((float)j / numSegments) * (2 * ew::PI);
                float x = r * cos(theta);
                float z = r * sin(theta);

                ew::Vertex vertex;
                vertex.pos = { x, y, z };
                vertex.normal = ew::Normalize(vertex.pos);
                vertex.uv = { static_cast<float>(j) / numSegments, static_cast<float>(i) / numSegments };

                meshData.vertices.push_back(vertex);
            }
        }


        int columns = numSegments + 1;
        for (int i = 0; i < numSegments; i++) {
            for (int j = 0; j < numSegments; j++) {
                int start = i * columns + j;

                meshData.indices.push_back(start);
                meshData.indices.push_back(start + 1);
                meshData.indices.push_back(start + columns);

                meshData.indices.push_back(start + columns + 1);
                meshData.indices.push_back(start + columns);
                meshData.indices.push_back(start + 1);
            }
        }

        return meshData;
    }

    ew::MeshData createCylinder(float height, float radius, int numSegments)
    {
        ew::MeshData meshData;

        float topY = height / 2; // y=0 is centered
        float bottomY = -topY;

        // Top center
        meshData.vertices.push_back({ { 0, topY, 0 }, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } });

        // Top ring
        float thetaStep = 2 * ew::PI / numSegments;
        for (int i = 0; i <= numSegments; i++) {
            float theta = i * thetaStep;
            ew::Vertex v;
            v.pos.x = cos(theta) * radius;
            v.pos.z = sin(theta) * radius;
            v.pos.y = topY;
            v.normal = { 0.0f, 1.0f, 0.0f };
            v.uv = { static_cast<float>(cos(theta) * 0.5f + 0.5f), 1};
            meshData.vertices.push_back(v);
        }

        // Bottom ring
        int bottomStart = meshData.vertices.size();
        for (int i = 0; i <= numSegments; i++) {
            float theta = i * thetaStep;
            ew::Vertex v;
            v.pos.x = cos(theta) * radius;
            v.pos.z = sin(theta) * radius;
            v.pos.y = bottomY;
            v.normal = { 0.0f, -1.0f, 0.0f };
            v.uv = { static_cast<float>(cos(theta) * 0.5f + 0.5f), 0 };
            meshData.vertices.push_back(v);
        }

        // Bottom center
        meshData.vertices.push_back({ { 0, bottomY, 0 }, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f } });

        // Create side faces
        int topRingStart = 1;
        int bottomRingStart = bottomStart + 1;
        int columns = numSegments + 1;

        for (int i = 0; i < numSegments; i++) {
            int start = topRingStart + i;
            // Triangle 1
            meshData.indices.push_back(start);
            meshData.indices.push_back(start + 1);
            meshData.indices.push_back(bottomRingStart + i);

            // Triangle 2
            meshData.indices.push_back(start + 1);
            meshData.indices.push_back(bottomRingStart + i + 1);
            meshData.indices.push_back(bottomRingStart + i);

            ew::Vec3 sideFaceNormal = ew::Normalize(ew::Cross(meshData.vertices[start].pos - meshData.vertices[start + 1].pos, meshData.vertices[start].pos - meshData.vertices[bottomRingStart + i].pos));
            meshData.vertices[start].normal = sideFaceNormal;
            meshData.vertices[start + 1].normal = sideFaceNormal;
            meshData.vertices[bottomRingStart + i].normal = sideFaceNormal;
            meshData.vertices[bottomRingStart + i + 1].normal = sideFaceNormal;
        }

        // Create top and bottom faces
        for (int i = 0; i < numSegments; i++) {
            meshData.indices.push_back(0);
            meshData.indices.push_back(topRingStart + i + 1);
            meshData.indices.push_back(topRingStart + i);
        }

        for (int i = 0; i < numSegments; i++) {
            meshData.indices.push_back(meshData.vertices.size() - 1);
            meshData.indices.push_back(bottomRingStart + i);
            meshData.indices.push_back(bottomRingStart + i + 1);
        }

        return meshData;
    }

}