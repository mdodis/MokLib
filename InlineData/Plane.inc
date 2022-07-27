
static float Plane2DV_Vertices[] = {
    // Positions  // Uvs
    -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, +0.5f, 0.0f, 1.0f,
    +0.5f, +0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f,
    +0.5f, +0.5f, 1.0f, 1.0f,
    +0.5f, -0.5f, 1.0f, 0.0f
};

const u32 Plane2DV_Vertices_Stride =
    /* Position      */ sizeof(float) * 3 +
    /* Texture Coord */ sizeof(float) * 2;