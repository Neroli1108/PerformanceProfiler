#include "PerformanceProfiler.h"
#include <iomanip>

//helper function
template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

//Implement all functions in PerformanceProfiler header file

//Implementation of FileManager class
FileManager::FileManager(std::string src, std::string dst)
{
    srcFile = new std::fstream(src.c_str());
    dstFile = new std::ofstream(dst.c_str());
}
FileManager::FileManager()
{
    srcFile = NULL;
    dstFile = NULL;
}
FileManager::~FileManager()
{
    delete srcFile;
    delete dstFile;
    srcFile = NULL;
    dstFile = NULL;
    storeFiles.clear();
}

std::map<FrameId, OrignalData> &FileManager::getStoreFiles()
{
    return this->storeFiles;
}

SummaryData &FileManager::getStoreSummaryData()
{
    return this->sd;
}

void FileManager::recordSummaryData()
{

    std::time_t t = std::time(0);
    std::tm *now = localtime(&t);
    *dstFile << "Date " << ToString(now->tm_mon + 1) << "/" << ToString(now->tm_mday) << "/" << ToString(now->tm_year + 1900) << "\t"
             << "File:" << sd.dataName << "\t"
             << "SizeOfData: " << sd.size << std::endl;
    *dstFile << "Stat Name"
             << "\t"
             << "Target"
             << "\t"
             << "Actual"
             << "\t"
             << "Delta(%)" << std::endl;
    *dstFile << "Frame Time(ms)"
             << "\t" << ToString(TargetFrameTime) << "\t" << ToString(sd.ActualFrameTime) << "\t" << std::fixed << std::setprecision(2) << (sd.FrameTimeDelta * 100) << "%" << std::endl;
    if (sd.FrameTimeBound == "GPU Bound")
    {
        *dstFile << " -Game Thread"
                 << "\t"
                 << "\t" << ToString(sd.ActualGameThread) << "\t"
                 << "\t" << std::endl;
        *dstFile << " -Render Thread"
                 << "\t"
                 << "\t" << ToString(sd.ActualRenderThread) << "\t"
                 << "\t" << std::endl;
        *dstFile << " -GPU"
                 << "\t"
                 << "\t" << ToString(sd.ActualGPU) << "\t" << sd.FrameTimeBound << std::endl;
    }
    else if (sd.FrameTimeBound == "Game Thread Bound")
    {

        *dstFile << " -Game Thread"
                 << "\t"
                 << "\t" << ToString(sd.ActualGameThread) << "\t" << sd.FrameTimeBound << std::endl;
        *dstFile << " -Render Thread"
                 << "\t"
                 << "\t" << ToString(sd.ActualRenderThread) << "\t"
                 << "\t" << std::endl;
        *dstFile << " -GPU"
                 << "\t"
                 << "\t" << ToString(sd.ActualGPU) << "\t"
                 << "\t" << std::endl;
    }
    else
    {
        *dstFile << " -Game Thread"
                 << "\t"
                 << "\t" << ToString(sd.ActualGameThread) << "\t"
                 << "\t" << std::endl;
        *dstFile << " -Render Thread"
                 << "\t"
                 << "\t" << ToString(sd.ActualRenderThread) << "\t" << sd.FrameTimeBound << std::endl;
        *dstFile << " -GPU"
                 << "\t"
                 << "\t" << ToString(sd.ActualGPU) << "\t"
                 << "\t" << std::endl;
    }

    *dstFile << "Triangles Drawn"
             << "\t" << ToString(TargetTriganlesDrawn) << "\t" << ToString(sd.ActualTrianglesDrawn) << "\t" << std::fixed << std::setprecision(2) << (sd.TrianglesDrawnDelta * 100) << "%" << std::endl;
    *dstFile << "Mesh Drawn Calls"
             << "\t" << ToString(TargetMeshDrawCalls) << "\t" << ToString(sd.ActualMeshDrawCalls) << "\t" << std::fixed << std::setprecision(2) << (sd.MeshDrawCallsDelta * 100) << "%" << std::endl;
}

std::fstream *FileManager::getSrcFile()
{
    return this->srcFile;
}

void FileManager::setSrcFile(std::string srcAddress)
{
    srcFile = new std::fstream(srcAddress.c_str());
}

//Implement the ConverterOperation class

void ConverterOperation::storeCSVData(std::fstream *srcfile, std::map<FrameId, OrignalData> &storeFiles)
{
    OrignalData fileData;
    std::string value;
    std::vector<std::string> temporaryStore;
    int firstLine = 1;
    while (srcfile->good())
    {
        if (firstLine)
        {
            std::getline(*srcfile, value);
            firstLine--;
        }
        else
        {
            int counter = 0;
            while (std::getline(*srcfile, value))
            {

                std::stringstream ss(value);
                std::string subvalue;

                while (std::getline(ss, subvalue, ','))
                {
                    if (std::find(temporaryStore.begin(), temporaryStore.end(), subvalue) == temporaryStore.end())
                        temporaryStore.push_back(subvalue);
                    counter++;

                    if (counter == 18)
                    {
                        counter = 0;
                        fileData.ID = std::atoi(temporaryStore[0].c_str());
                        for (int i = 1; i < temporaryStore.size(); i++)
                        {
                            if (temporaryStore[i] == "STAT_StaticMeshTriangles")
                            {
                                fileData.STAT_StaticMeshTriangles = std::atof(temporaryStore[++i].c_str());
                            }
                            else if (temporaryStore[i] == "STAT_MeshDrawCalls")
                            {
                                fileData.STAT_MeshDrawCalls = std::atof(temporaryStore[++i].c_str());
                            }
                            else if (temporaryStore[i] == "Stat_GPU_Total")
                            {
                                fileData.Stat_GPU_Total = std::atof(temporaryStore[++i].c_str());
                            }
                            else if (temporaryStore[i] == "GameThread")
                            {
                                fileData.GameThread = std::atof(temporaryStore[++i].c_str());
                            }
                            else if (temporaryStore[i] == "STAT_FrameTime")
                            {
                                fileData.STAT_FrameTime = std::atof(temporaryStore[++i].c_str());
                            }
                            else if (temporaryStore[i] == "STAT_TotalSceneRenderingTime")
                            {
                                fileData.STAT_TotalSceneRenderingTime = std::atof(temporaryStore[++i].c_str());
                            }
                        }
                        storeFiles[fileData.ID] = fileData;
                        temporaryStore.clear();
                    }
                }
            }
        }
    }
}

void ConverterOperation::calculateSummaryData(std::map<FrameId, OrignalData> &storeFiles, SummaryData &storeSummaryData, std::string name)
{
    float num = storeFiles.size();
    float totalSTAT_StaticMeshTriangles = 0.00;
    float totalSTAT_MeshDrawCalls = 0.00;
    float totalStat_GPU_Total = 0.00;
    float totalGameThread = 0.00;
    float totalSTAT_FrameTime = 0.00;
    float totalSTAT_TotalSceneRenderingTime = 0.00;
    storeSummaryData.size = num;
    for (std::map<FrameId, OrignalData>::iterator it = storeFiles.begin(); it != storeFiles.end(); it++)
    {
        totalSTAT_FrameTime += it->second.STAT_FrameTime;
        totalGameThread += it->second.GameThread;
        totalStat_GPU_Total += it->second.Stat_GPU_Total;
        totalSTAT_MeshDrawCalls += it->second.STAT_MeshDrawCalls;
        totalSTAT_TotalSceneRenderingTime += it->second.STAT_TotalSceneRenderingTime;
        totalSTAT_StaticMeshTriangles += it->second.STAT_StaticMeshTriangles;
    }
    storeSummaryData.dataName = name;
    storeSummaryData.ActualFrameTime = totalSTAT_FrameTime / num;
    storeSummaryData.ActualGPU = totalStat_GPU_Total / num;
    storeSummaryData.ActualGameThread = totalGameThread / num;
    storeSummaryData.ActualRenderThread = totalSTAT_TotalSceneRenderingTime / num;
    storeSummaryData.ActualMeshDrawCalls = totalSTAT_MeshDrawCalls / num;
    storeSummaryData.ActualTrianglesDrawn = totalSTAT_StaticMeshTriangles / num;

    storeSummaryData.MeshDrawCallsDelta = (storeSummaryData.ActualMeshDrawCalls - TargetMeshDrawCalls) / TargetMeshDrawCalls;
    storeSummaryData.FrameTimeDelta = (storeSummaryData.ActualFrameTime - TargetFrameTime) / TargetFrameTime;
    storeSummaryData.TrianglesDrawnDelta = (storeSummaryData.ActualTrianglesDrawn - TargetTriganlesDrawn) / TargetTriganlesDrawn;

    if (storeSummaryData.ActualGPU >= storeSummaryData.ActualRenderThread)
    {
        if (storeSummaryData.ActualGPU >= storeSummaryData.ActualGameThread)
        {
            storeSummaryData.FrameTimeBound = "GPU Bound";
        }
        else if (storeSummaryData.ActualRenderThread >= storeSummaryData.ActualGameThread)
        {
            storeSummaryData.FrameTimeBound = "Render Thread Bound";
        }
        else
        {
            storeSummaryData.FrameTimeBound = "Game Thread Bound";
        }
    }
    else
    {
        if (storeSummaryData.ActualRenderThread >= storeSummaryData.ActualGameThread)
        {
            storeSummaryData.FrameTimeBound = "Render Thread Bound";
        }
        else
        {
            storeSummaryData.FrameTimeBound = "Game Thread Bound";
        }
    }
}

OrignalData ConverterOperation::retriveNthData(int n, std::map<FrameId, OrignalData> &storeFiles)
{
    std::map<FrameId, OrignalData>::iterator it = storeFiles.begin();
    while (n > 1)
    {
        it++;
        n--;
    }

    return it->second;
}

//implement the Converter function
Converter::Converter()
{
    this->fm = new FileManager();
    this->co = new ConverterOperation();
}
Converter::Converter(std::string src, std::string dst)
{
    this->srcAddress = src;
    this->dstAddress = dst;
    this->fm = new FileManager(this->srcAddress, this->dstAddress);
    this->co = new ConverterOperation();
}
Converter::Converter(std::string src, int num)
{
    this->fm = new FileManager();
    this->fm->setSrcFile(src);
    this->co = new ConverterOperation();
    this->specific = num;
}
Converter::~Converter()
{
    this->fm = NULL;
    this->co = NULL;
}

FileManager *Converter::getFileManager()
{
    return this->fm;
}
void Converter::storeData()
{
    this->co->storeCSVData(this->fm->getSrcFile(), this->fm->getStoreFiles());
}

void Converter::writeSummary()
{
    std::string name = srcAddress.substr(srcAddress.find_last_of("/\\") + 1);
    this->storeData();
    this->co->calculateSummaryData(this->fm->getStoreFiles(), this->fm->getStoreSummaryData(), name);
    this->fm->recordSummaryData();
}

void Converter::showSpecificdata(int n)
{
    this->storeData();
    OrignalData od = this->co->retriveNthData(n, this->fm->getStoreFiles());
    std::cout << "Frame ID: " << od.ID << std::endl;
    std::cout << "STAT_StaticMeshTriangles: " << od.STAT_StaticMeshTriangles << std::endl;
    std::cout << "STAT_MeshDrawCalls: " << od.STAT_MeshDrawCalls << std::endl;
    std::cout << "Stat_GPU_Total: " << od.Stat_GPU_Total << std::endl;
    std::cout << "GameThread: " << od.GameThread << std::endl;
    std::cout << "STAT_FrameTime: " << od.STAT_FrameTime << std::endl;
    std::cout << "STAT_TotalSceneRenderingTime: " << od.STAT_TotalSceneRenderingTime << std::endl;
}