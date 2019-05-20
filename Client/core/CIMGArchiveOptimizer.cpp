#include "StdInc.h"
#include "CIMGArchive.h"
/*
GUIEditor.window[1] = guiCreateWindow(499, 255, 366, 177, "Optimize gta3.img", false)
guiWindowSetSizable(GUIEditor.window[1], false)

GUIEditor.label[1] = guiCreateLabel(11, 28, 349, 57, , false, GUIEditor.window[1])
guiSetFont(GUIEditor.label[1], "default-bold-small")
guiLabelSetHorizontalAlign(GUIEditor.label[1], "left", true)

GUIEditor.button[1] = guiCreateButton(112, 108, 125, 47, "GENERATE", false, GUIEditor.window[1])    
*/

CIMGArchiveOptimizer::CIMGArchiveOptimizer()
{
    CreateGUI();
}

void CIMGArchiveOptimizer::CreateGUI()
{
    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    CVector2D windowSize = CVector2D(499, 255);
    CGUI* pManager = g_pCore->GetGUI();

    CGUIWindow* pWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, _("IMG OPTIMIZER")));
    pWindow->SetCloseButtonEnabled(true);
    pWindow->SetMovable(true);
    pWindow->SetPosition((resolution - windowSize) / 2);
    pWindow->SetSize(CVector2D(366, 177));
    pWindow->SetSizingEnabled(false);
    pWindow->SetAlwaysOnTop(true);
    pWindow->BringToFront();
    pWindow->SetVisible(false);
    m_pWindow = pWindow;

    SString strImgSpaceRequiredText = "A custom optimized gta3.img file needs to be generated in your models folder. \
                Please make sure you have the required space. The size of the custom gta3.img will be a little bigger \
                than your current one. ";

    m_pImgSpaceRequiredLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _(strImgSpaceRequiredText)));
    m_pImgSpaceRequiredLabel->SetPosition(CVector2D(11, 28));
    m_pImgSpaceRequiredLabel->SetSize(CVector2D(349, 57));
    m_pImgSpaceRequiredLabel->SetFont("default-bold-small");
    m_pImgSpaceRequiredLabel->SetHorizontalAlign(CGUI_ALIGN_LEFT);

    CGUIButton* pImgGenerateButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, _("GENERATE")));
    pImgGenerateButton->SetClickHandler(GUI_CALLBACK(&CIMGArchiveOptimizer::OnImgGenerateClick, this));
    pImgGenerateButton->SetSize(CVector2D(125, 47));
    pImgGenerateButton->SetPosition(CVector2D(112, 108));
    pImgGenerateButton->SetZOrderingEnabled(false);
}

void CIMGArchiveOptimizer::SetVisible(bool bVisible)
{
    if (bVisible)
    {
        if (!m_pWindow->IsVisible())
        {
            std::printf("yeah, making it visible\n");
            m_pWindow->SetVisible(bVisible);
            m_pWindow->SetVisible(bVisible);
            m_pWindow->BringToFront();
            m_pWindow->SetZOrderingEnabled(false);
        }
    }
    else
    {
        m_pWindow->SetVisible(bVisible);
    }
}


/*
CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD("C:\\Users\\danish\\Desktop\\gtaTxdDff\\Infernus\\infernus.txd", CBuffer());
pRenderWare->WriteTXD("C:\\Users\\danish\\Desktop\\gtaTxdDff\\Infernus\\infernus2.txd", pTxdDictionary);

std::vector <CTextureInfo> vecTexturesInfo;

SString theDirPath = "C:\\Program Files (x86)\\NVIDIA Corporation\\Texture Atlas Tools\\DEMOS\\Direct3D9\\bin\\release\\";
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "grass.png", 256, 256)); //grass.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "nvidia_cloth.png", 512, 512)); //nvidia_cloth.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "1d_debug.png", 128, 128)); //1d_debug.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "shine.png", 64, 64)); //shine.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "mtasa.png", 800, 600)); //mtasa.png

auto pTextureInfo = g_pCore->CreateTextureAtlas(vecTexturesInfo);
*/


DWORD GetActualFileSize(DWORD blockSize)
{
    const uint64_t IMG_BLOCK_SIZE = 2048;
    blockSize = (blockSize + IMG_BLOCK_SIZE - 1) & ~(IMG_BLOCK_SIZE - 1); // Round up to block size
    return blockSize;
}

void OptimizeTXDFile(CIMGArchiveFile* newFile)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto RwTexDictionaryStreamGetSize = (unsigned int(__cdecl*)(RwTexDictionary *dict))0x804930;

    RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD(nullptr, newFile->fileByteBuffer, false);
    if (pTxdDictionary)
    {
       // std::printf("txd loaded\n");
        unsigned int txdSize = RwTexDictionaryStreamGetSize(pTxdDictionary);

        //free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(txdSize);
        newFile->fileEntry->fSize = newFile->actualFileSize / 2048;
        std::printf("txd file: %s  | new txd size: %d | actualFileSize: %d | capacity: %d\n",
            newFile->fileEntry->fileName, txdSize, (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        pRenderWare->WriteTXD(newFile->fileByteBuffer.GetData(), newFile->actualFileSize, pTxdDictionary);

        pRenderWare->DestroyTXD(pTxdDictionary);
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry->fileName);
    }
}

void OptimizeDFFFile(CIMGArchiveFile* newFile)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto RpClumpStreamGetSize = (unsigned int(__cdecl*)(RpClump *))0x74A5E0;

    
    RwTexDictionary* pTxdDictionary =  nullptr;
    /*pTxdDictionary = pRenderWare->ReadTXD("dyno_box.txd", CBuffer(), false);
    if (pTxdDictionary)
    {
        std::printf("txd loaded\n");
    }*/

   // RpClump* pClump = pRenderWare->ReadDFF("cj_bag_det.dff", CBuffer(), 0, false, pTxdDictionary);
    RpClump* pClump = pRenderWare->ReadDFF(newFile->fileEntry->fileName, newFile->fileByteBuffer, 0, false);
    if (pClump)
    {
        unsigned int clumpSize = RpClumpStreamGetSize(pClump);

        //clumpSize += 10000;

        //free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(clumpSize);
        newFile->fileEntry->fSize = newFile->actualFileSize / 2048;
        std::printf("dff file: %s  | new dff size: %u | actualFileSize: %d | capacity: %d\n",
            newFile->fileEntry->fileName, clumpSize, (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        void* pData = newFile->fileByteBuffer.GetData();
       // pRenderWare->WriteDFF(pData, newFile->actualFileSize, pClump);

       SString strPathOfGeneratedDff = "dffs\\";
        pRenderWare->WriteDFF(strPathOfGeneratedDff + newFile->fileEntry->fileName, pClump);

        pRenderWare->DestroyDFF(pClump);
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry->fileName);
    }
}

bool CIMGArchiveOptimizer::OnImgGenerateClick(CGUIElement* pElement)
{
    std::printf("Generate button pressed\n");

    CIMGArchive* newIMgArchive = new CIMGArchive("models\\gta3.img", IMG_FILE_READ);
    newIMgArchive->ReadEntries();

    CIMGArchive* newIMgArchiveOut = new CIMGArchive("proxy_test_gta3.img", IMG_FILE_WRITE);
    std::vector<CIMGArchiveFile*> imgArchiveFiles;
    for (DWORD i = 0; i < newIMgArchive->GetFileCount(); i++)
    {
        CIMGArchiveFile* newFile = newIMgArchive->GetFileByID(i);
        if (newFile != NULL)
        {
            SString strFileName = newFile->fileEntry->fileName;
            SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);
            
            // check if it's a txd file
            if (strFileExtension == "txd")
            {
                //OptimizeTXDFile(newFile);
            }
            else if (strFileExtension == "dff")
            {
               
                OptimizeDFFFile(newFile);
            }
            
            imgArchiveFiles.push_back(newFile);
        }
    }

    newIMgArchiveOut->WriteEntries(imgArchiveFiles);

    //delete newFile;
    delete newIMgArchive;
    delete newIMgArchiveOut;

    return true;
}

