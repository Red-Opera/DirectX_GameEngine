cbuffer ObjectTransform : register(b0)
{
    matrix model;
    matrix cameraTransform;
    matrix worldViewProjection;
};