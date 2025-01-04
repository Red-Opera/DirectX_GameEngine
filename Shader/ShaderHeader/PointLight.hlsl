cbuffer LightInfoConstant
{
    float3 lightViewPosition;
    float3 ambient;             // ÁÖº¯±¤ °­µµ
    float3 diffuseColor;        // ºÐ»ê±¤ »ö»ó
    float diffuseIntensity;     // ºÐ»ê±¤ °­µµ
    float attConst;             // ºû °¨¼è »ó¼ö
    float attLin;               // ºû °­¼è ¼±Çü °¨¼Ò
    float attQuad;              // ºû »ó¼è Á¦°ö °¨¼Ò
};    