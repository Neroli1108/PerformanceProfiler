#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <ostream>
#include <ctime>
#include <algorithm>

#define TargetFrameTime 100
#define TargetTriganlesDrawn 18000000
#define TargetMeshDrawCalls 18000

typedef int FrameId;

//store the CSV file original data
struct OrignalData
{ // for identify every frame data
    FrameId ID;

    float STAT_StaticMeshTriangles;
    float STAT_MeshDrawCalls;
    float Stat_GPU_Total;
    float GameThread;
    float STAT_FrameTime;
    float STAT_TotalSceneRenderingTime;
};

//store the data after being calculated
struct SummaryData
{
    std::string dataName;
    int size;
    float ActualFrameTime;
    float ActualGameThread;
    float ActualRenderThread;
    float ActualGPU;
    float ActualTrianglesDrawn;
    float ActualMeshDrawCalls;
    float FrameTimeDelta;
    float TrianglesDrawnDelta;
    float MeshDrawCallsDelta;
    std::string FrameTimeBound;

    //record the date
};

// open the specific directory and iterate all CSV files
//
class FileManager
{
  public:
    FileManager();
    ~FileManager();
    FileManager(std::string Sourceaddress, std::string DestAddress);
    std::map<FrameId, OrignalData> &getStoreFiles();
    SummaryData &getStoreSummaryData();
    void recordSummaryData();
    std::fstream *getSrcFile();
    void setSrcFile(std::string);

  private:
    std::map<FrameId, OrignalData> storeFiles;
    SummaryData sd;
    std::fstream *srcFile;
    std::ostream *dstFile;
};

//Operate the CSV files
class ConverterOperation
{
  public:
    void storeCSVData(std::fstream *srcFile, std::map<FrameId, OrignalData> &storeFiles);
    void calculateSummaryData(std::map<FrameId, OrignalData> &storeFiles, SummaryData &storeSummaryData, std::string name);
    OrignalData retriveNthData(int n, std::map<FrameId, OrignalData> &storeFiles);
};

class Converter
{
  public:
    Converter();
    ~Converter();
    Converter(std::string src, std::string dst);
    Converter(std::string src, int num);
    void storeData();
    void writeSummary();
    void showSpecificdata(int n);
    FileManager *getFileManager();

  private:
    FileManager *fm;
    ConverterOperation *co;
    std::string srcAddress;
    std::string dstAddress;
    int specific;
};
