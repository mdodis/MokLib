#include "OBJ.h"
#include "Base.h"
#include "Debugging/Base.h"
#include "Importers/Import.h"
#include "Memory/Base.h"
#include "Parsing.h"
#include "Memory/Arena.h"

namespace OBJDeclaration {
    enum Type {
        Invalid = 0,
        VertexTexture,
        VertexNormal,
        Vertex,
        Face,
        UseMtl,
        SetSmoothing,
        Comment,
        Object,
        MtlLib,
        Count
    };
};

Str OBJ_Declarations[OBJDeclaration::Count] = {
    Str::NullStr,
    LIT("vt"),
    LIT("vn"),
    LIT("v"),
    LIT("f"),
    LIT("usemtl"),
    LIT("s"),
    LIT("#"),
    LIT("o"),
    LIT("mtllib"),
};

#pragma pack(push, 1)
struct OBJVertex {
    Vec3 v;
    Vec3 n;
    Vec2 t;
};
#pragma pack(pop)

struct OBJIndex {
    u32 indices[3];
};

bool operator==(const OBJIndex &left, const OBJIndex &right) {
    return (left.indices[0] == right.indices[0])
        && (left.indices[1] == right.indices[1])
        && (left.indices[2] == right.indices[2]);
}

PROC_IMPORTER_LOAD(import_obj_load) {
    FileTape tape(file_handle);

    CREATE_SCOPED_ARENA(get_system_allocator(), temp_arena, Arena::Default_Size);

    TArray<Vec3> vertices(temp_arena.to_alloc());
    TArray<Vec2> textures(temp_arena.to_alloc());
    TArray<Vec3> normals(temp_arena.to_alloc());
    TArray<OBJIndex> encoded_indices(temp_arena.to_alloc());
    TArray<u32> indices(temp_arena.to_alloc());

    TArray<OBJVertex> output_vertices(alloc);

    bool has_ended = false;

    while (!tape.end() && !has_ended) {
        eat_whitespace(&tape);

        CREATE_INLINE_ARENA(in_arena, 1024);
        IAllocator in_alloc = in_arena.to_alloc();

        Str declaration = parse_string(&tape, in_alloc);

        u32 match_result = match_strings(declaration, OBJ_Declarations, OBJDeclaration::Count);
        if (match_result == OBJDeclaration::Count) {
            return false;
        }

        OBJDeclaration::Type match_decl = (OBJDeclaration::Type)match_result;

        switch (match_decl) {

            case OBJDeclaration::Vertex: {
                eat_space(&tape);

                float x, y, z;
                parse_num(&tape, x);
                eat_space(&tape);
                parse_num(&tape, y);
                eat_space(&tape);
                parse_num(&tape, z);

                vertices.add(Vec3 {x , y, z});
            } break;

            case OBJDeclaration::VertexTexture: {
                eat_space(&tape);

                float u, v;
                parse_num(&tape, u);
                eat_space(&tape);
                parse_num(&tape, v);

                textures.add(Vec2 {u, v});
            } break;

            case OBJDeclaration::VertexNormal: {
                eat_space(&tape);

                float u, v, w;
                parse_num(&tape, u);
                eat_space(&tape);
                parse_num(&tape, v);
                eat_space(&tape);
                parse_num(&tape, w);

                normals.add(Vec3 {u, v, w});
            } break;

            case OBJDeclaration::Face: {
                eat_space(&tape);

                // @todo: handle triangulated models
                u32 index_v[4];
                u32 index_t[4];
                u32 index_n[4];

                for (i32 i = 0; i < 4; ++i) {
                    parse_num(&tape, index_v[i]); tape.peek_char('/');
                    parse_num(&tape, index_t[i]); tape.peek_char('/');
                    parse_num(&tape, index_n[i]); tape.peek_char('/');
                    eat_space(&tape);
                }

                OBJIndex faces[6] = {
                    {{index_v[0], index_t[0], index_n[0]}},
                    {{index_v[1], index_t[1], index_n[1]}},
                    {{index_v[2], index_t[2], index_n[2]}},
                    {{index_v[2], index_t[2], index_n[2]}},
                    {{index_v[3], index_t[3], index_n[3]}},
                    {{index_v[0], index_t[0], index_n[0]}},
                };


                for (i32 i = 0; i < 6; ++i) {
                    i32 index_of = encoded_indices.index_of(faces[i]);
                    if (index_of == -1) {
                        index_of = encoded_indices.add(faces[i]);

                        OBJVertex vertex;
                        vertex.v = vertices[faces[i].indices[0] - 1];
                        vertex.t = textures[faces[i].indices[1]  - 1];
                        vertex.n = normals[faces[i].indices[2] - 1];

                        output_vertices.add(vertex);
                    }

                    indices.add(index_of);
                }

            } break;

            case OBJDeclaration::UseMtl: {
                eat_space(&tape);

                Str material_name = parse_string(&tape, in_alloc);
                // DEBUG_PRINTF("UseMtl: %.*s", material_name.len, material_name.data);
            } break;

            case OBJDeclaration::SetSmoothing: {
                eat_space(&tape);

                Str is_on_str = parse_string(&tape, in_alloc);
                // DEBUG_PRINTF("Set smoothing: %.*s", is_on_str.len, is_on_str.data);
            } break;

            case OBJDeclaration::Comment: {
                eat_line(&tape);
                // DEBUG_PRINTF("%s", "# ...");
            } break;

            case OBJDeclaration::MtlLib: {
                eat_space(&tape);
                Str filename = parse_string(&tape, in_alloc);
            } break;

            case OBJDeclaration::Object: {
                eat_space(&tape);
                Str object_name = parse_string(&tape, in_alloc);
            } break;

            default: {
                has_ended = true;
            } break;
        }
    }

    u64 copy_size = indices.size * sizeof(indices[0]);
    umm output_indices = alloc.reserve(alloc.context, copy_size);

    memcpy(output_indices, indices.data, copy_size);

    result->model.num_vertices = output_vertices.size;
    result->model.num_indices  = indices.size;
    result->model.ptr_vertices = (umm)output_vertices.data;
    result->model.ptr_indices  = output_indices;
    result->kind = ImportKind::Model;
    return true;
}
