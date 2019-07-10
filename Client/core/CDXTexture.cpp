#pragma once

#include "StdInc.h"
#include "CDXTexture.h"

CDXTexture::CDXTexture()
{
    Initialize();
}

CDXTexture::CDXTexture(RwTexture* pTexture)
{
    Initialize();

    CRenderWare* pRenderware = CCore::GetSingleton().GetGame()->GetRenderWare();

    m_pTexture = pTexture;
    _rwD3D9RasterExt* rasterExt = pRenderware->GetRasterExt(m_pTexture->raster);
    if (rasterExt->d3dFormat != m_kTextureFormat)
    {
        m_pTexture = pRenderware->RwTextureCreateWithFormat(m_pTexture, m_kTextureFormat);
        rasterExt = pRenderware->GetRasterExt(m_pTexture->raster);
    }
    dxTexture = rasterExt->texture;
    imageWidth = m_pTexture->raster->width;
    imageHeight = m_pTexture->raster->height;
}

CDXTexture::~CDXTexture()
{
    UnlockTexture();

    if (bNewRwTextureCreated)
    {
        // TODO: delete m_pTexture using an Rw function
    }
}

void CDXTexture::SaveTextureToFile(std::string& name)
{
    UnlockTexture();

    HRESULT hr = D3DXSaveTextureToFile(("C:\\Users\\danish\\Desktop\\" + name + ".png").c_str(), D3DXIFF_PNG, dxTexture, NULL);
    if (FAILED(hr))
    {
        printf("failed to save texture file | %s | hr = %#.8x\n", ("C:\\Users\\danish\\Desktop\\" + name).c_str(), hr);
    }
}

void CDXTexture::Initialize()
{
    dxTexture = nullptr;
    bNewRwTextureCreated = false;
    bTextureLocked = false;
    pixels = nullptr;
    imageWidth = 0;
    imageHeight = 0;
    m_pixelSizeInBytes = 4;
    memset(&textureLockedRect, 0, sizeof(D3DLOCKED_RECT));
}

void* CDXTexture::GetPixel(int x, int y)
{
    unsigned int theOffset = ((m_pixelSizeInBytes * x) + (m_pixelPitch * y));
    return (void*)(pixels + theOffset);
}

bool CDXTexture::LockTexture(DWORD Flags)
{
    RECT textureRect;
    textureRect.left = textureRect.top = 0;
    textureRect.right = imageWidth;
    textureRect.bottom = imageHeight;

    HRESULT hr = dxTexture->LockRect(0, &textureLockedRect, &textureRect, Flags);
    if (hr != S_OK)
    {
        printf("pTexture->LockRect: failed to lock textureRect\n");
        return false;
    }

    pixels = (unsigned char*)textureLockedRect.pBits;
    m_pixelPitch = textureLockedRect.Pitch;
    bTextureLocked = true;
    return true;
}

void CDXTexture::UnlockTexture()
{
    if (bTextureLocked)
    {
        HRESULT hr = dxTexture->UnlockRect(0);
        assert(hr == S_OK);
        bTextureLocked = false;
    }
}

bool CDXTexture::CreateTextureLocked(unsigned int width, unsigned int height, DWORD lockFlags)
{
    imageWidth = width;
    imageHeight = height;
    IDirect3DDevice9* pDevice = g_pGraphics->GetDevice();
    HRESULT           hr = pDevice->CreateTexture(imageWidth, imageHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &dxTexture, NULL);
    if (FAILED(hr))
    {
        printf("pD3DDevice->CreateTexture fail\n");
        return false;
    }
    else
    {
        printf("texture loaded with width, height = %u, %u\n", imageWidth, imageHeight);
        if (!LockTexture(lockFlags))
        {
            printf("LockTexture() fail \n");
            return false;
        }
    }

    return true;
}

void CDXTexture::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    unsigned int theOffset = ((m_pixelSizeInBytes * x) + (m_pixelPitch * y));
    PixelColor*  pPixel = (PixelColor*)(pixels + theOffset);
    pPixel->a = alpha;
    pPixel->r = red;
    pPixel->g = green;
    pPixel->b = blue;
}

void CDXTexture::setPixel(int x, int y, PixelColor* thePixel)
{
    unsigned int theOffset = ((m_pixelSizeInBytes * x) + (m_pixelPitch * y));
    PixelColor*  pPixel = (PixelColor*)(pixels + theOffset);
    *pPixel = *thePixel;
}

void CDXTexture::fillPixels(int red, int green, int blue)
{
    for (int y = 0; y < imageHeight; y++)
    {
        for (int x = 0; x < imageWidth; x++)
        {
            setPixel(x, y, red, green, blue, 255);
        }
    }
}