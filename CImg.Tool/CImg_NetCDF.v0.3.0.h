#ifndef CIMG_NETCDF
#define CIMG_NETCDF

#include <iostream>
#include <string>
#include <vector>
#include "../CImg/CImg.h"
#include <netcdfcpp.h>
#include "../NetCDF.Tool/NetCDFinfo.h"

#ifdef cimg_debug
#undef cimg_debug
#endif
#define cimg_debug 1


using namespace std;
using namespace cimg_library;

/*
#ifndef cimg_debug
  #define cimg_display_type  0
  #define cimg_debug         20
  #include "../../CImg/useCImg.ini.h"
  using namespace cimg_library;
  #if ( defined(_MSC_VER) && _MSC_VER<=1200 ) || defined(__DMC__)
    #define std
  #endif
#endif
*/
#define NC_ERROR   -2
#define CODE_ERROR -3
#define DIM_ERROR  -4

//! add NetCDF read/write support to CImg class
/**
 * an object of this class may be created or CImg may derived from this class to handle NetCDF
 * \li object in \c .cpp
 * \code
 *   CImgNetCDF<int> object;
 * \endcode
 * \li derivation in \c .h
 * \code
 *   template<typename T> CnewClass: \c CImg<T>, \c CImgNetCDF<T>
 * \endcode
 *
 * \note \c CImgNetCDF and \c CImg must be of the same type
 * \note For this implementation, NetCDF variable dimensions need to be 5: four fixed dimensions (x,y,z,v) and one unlimited dimension (t)
 *
 * \codevpNCDim
 *   
 * \endcode
 *
 * \see CImg
 *
**/
template<typename T> class CImgNetCDF
{
//variable
  public:
  //! pointer to NetCDF error handling
  /**
   * 
   *
   * \code
   *   NcError err(NcError::verbose_nonfatal);
   * \endcode
   *
   * \see 
   *
  **/
  NcError *pNCError;
  //! pointer to NetCDF file
  /**
   * 
   *
   * \code
   *   string f="2D3C.nc";
   *   NcFile dataFile(f.c_str(),NcFile::ReadOnly);//Read
   *   NcFile dataFile(f.c_str(),NcFile::Replace);//Write
   * \endcode
   *
   * \see 
   *
  **/
  NcFile *pNCFile;
  //! pointers to NetCDF dimensions
  /**
   * 
   *
   * \code
   *   if (!(pDimi=dataFile.get_dim("dim_x"))) return NC_ERROR;//Read
   *   if (!(pDimi=dataFile.add_dim("dim_x",dimi))) return NC_ERROR;//Write
   * \endcode
   *
   * \see loadDimx,y,z,v LOAD_DIM
   *
  **/
	std::vector<NcDim*>vpNCDim;
  //! virtual functions which may return CImg dimensions
  /**
   * To use this functions, CImg may derive from this class.
   *
   * \code
   *   dataFile.dimx()
   * \endcode
   *
   * \see loadDimTime loadDimx,y,z,v from LOAD_DIM checkDimx,y,z,v(CImg) from CHECK_DIM
   *
  **/
#ifdef DIM
#undef DIM
#endif
#define DIM(X) int dim##X(void){return 0;}
	DIM(x)
	DIM(y)
	DIM(z)
	DIM(v)

	int dim(int d){
               	std::cerr << "CImgNetCDF::__dim__()" << d <<std::endl;
                return 0;
        }

  //! functions read dimensions from NetCDF file
  /**
   * 
   *
   * \code
   *   dataFile.loadDimx()
   * \endcode
   *
   * \see loadDimTime DIM CHECK_DIM
   *
  **/
#define LOAD_DIM(X,N) int loadDim##X(void) \
 { \
      std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl; \
      if(N>=vpNCDim.size()) return CODE_ERROR; \
      return (int)vpNCDim[N]->size(); \
}
  LOAD_DIM(x,0)
  LOAD_DIM(y,1)
  LOAD_DIM(z,2)
  LOAD_DIM(v,3)

	int loadDim(int d) {
#if cimg_debug>10
                 std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl;
#endif
                 if(d>=vpNCDim.size()) return CODE_ERROR;
                 return (int)vpNCDim[d]->size();
        }
  //! 
  /**
   * 
   *
   * \code
   *   if (!(pDimt=dataFile.get_dim("time"))) return NC_ERROR;//Read
   *   if (!(pDimt=dataFile.add_dim("time"))) return NC_ERROR;//Write: add an unlimited dimension
   * \endcode
   *
   * \see loadDimTime
   *
  **/
  NcDim *pNCDimt;
  //! pointers to NetCDF data
  /**
   * access to NetCDF data
   *
   * \code
   *   
   * \endcode
   *
  **/
  NcVar *pNCvar;
  //! function read unlimited dimension from NetCDF file
  /**
   * the unlimited time dimension may be a loaded one by one
   *
   * \code
   *   dataFile.loadDimTime()
   * \endcode
   *
   * \see loadDimx,y,z,v from LOAD_DIM
   *
  **/
  int loadDimTime(void)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl;
#endif
    if(pNCDimt==NULL) {std::cerr << "CImgNetCDF::" << __func__ << "() error: pNCDimt==NULL." << std::endl;return CODE_ERROR;}
    return (int)pNCDimt->size();
  }
//methods
  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  CImgNetCDF(void)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl;
#endif
    pNCError=new NcError(NcError::verbose_nonfatal);
    pNCFile=NULL;
    pNCDimt=NULL;
    pNCvar=NULL;
  }

  virtual ~CImgNetCDF(void)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl;
#endif
    //delete(pNCvar);
    if(pNCFile!=NULL){
	pNCFile->close();
	//delete(pNCFile);
	}
    if(pNCError!=NULL)delete(pNCError);
  }


  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFFile(char *fileName)//,std::vector<std::string> pDim_names=NULL,std::string *pTime_name=NULL)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(" << fileName << "," << std::endl;//;
//  std::cerr << "," << ((pTime_name==NULL)?"default":(*pTime_name).c_str()) << ")" << std::endl;
#endif
    pNCFile=new NcFile(fileName,NcFile::ReadOnly);
//    if(pDim_names==NULL)
      return !(pNCFile->is_valid());
//    else
//      return loadNetCDFDims(*pDim_names,pTime_name);
  }
  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  int saveNetCDFFile(char *fileName,std::vector<std::string> *pDim_names=NULL,std::string *pTime_name=NULL)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(" << fileName << ",";
//  std::cerr << ((dimi_name==NULL)?"default":dimi_name) << "," << ((dimj_name==NULL)?"default":dimj_name) << ",";
  std::cerr << ((pTime_name==NULL)?"":*pTime_name) << ")" << std::endl;
#endif
    pNCFile=new NcFile(fileName,NcFile::Replace);
    if( (pDim_names!=NULL) && (pTime_name!=NULL)) return addNetCDFDims(*pDim_names,*pTime_name);
    else return 0;
  }
  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  int setNetCDFFile(NcFile *pNetCDFFile)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(" << pNetCDFFile << ")" << std::endl;
#endif
    pNCFile=pNetCDFFile;
    return 0;
  }

  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  NcFile *getNetCDFFile(){
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(" << pNetCDFFile << ")" << std::endl;
#endif
    return pNCFile;
  }

  //! add variables in NetCDF file
  /**
   * save variables in NetCDF file, may be called after creation of dimensions \c CImgNetCDF::addNetCDFDims
   *
   * \code
   *   dimv=2;char *fileName="2D2C.nc";
   *   C2DnCNetCDFvelocity vel;//this class MUST be derivated as dimi(), dimj() and dimv() are VIRTUAL
   *   vel.pNCFile=new NcFile(fileName,NcFile::Replace);
   *   vel.addNetCDFDims("dimX","dimY","time");
   *   std::vector<std::string> var_names(dimv+1);
   *   var_names[0]="vel_X";var_names[1]="vel_Y";
   *   var_names[2]="flag";
   *   std::vector<std::string> unit_names(2);
   *   unit_names[0]="m/s";unit_names[1]="none";
   *   addNetCDFVar(&var_names,&unit_names);
   * \endcode
   *
   * \see CImgNetCDF::addNetCDFDims
   *
  **/
  int addNetCDFVar(std::string var_name,std::string unit_name)
  {
#if cimg_debug>10
  	std::cerr << "CImgNetCDF::" << __func__ << "(" << var_name << "," << unit_name << ")" << std::endl;
#endif
	switch(vpNCDim.size()){
		case 4:if (!(pNCvar=pNCFile->add_var(var_name.c_str(),(NcType)NcTypeInfo<T>::ncId(),pNCDimt,vpNCDim[3],vpNCDim[2],vpNCDim[1],vpNCDim[0]))) return NC_ERROR;break;
		case 3:if (!(pNCvar=pNCFile->add_var(var_name.c_str(),(NcType)NcTypeInfo<T>::ncId(),pNCDimt,vpNCDim[2],vpNCDim[1],vpNCDim[0]))) return NC_ERROR;break;
		case 2:if (!(pNCvar=pNCFile->add_var(var_name.c_str(),(NcType)NcTypeInfo<T>::ncId(),pNCDimt,vpNCDim[1],vpNCDim[0]))) return NC_ERROR;break;
		case 1:if (!(pNCvar=pNCFile->add_var(var_name.c_str(),(NcType)NcTypeInfo<T>::ncId(),pNCDimt,vpNCDim[0]))) return NC_ERROR;break;	
		default:NULL;
	}
    //Define units attributes for data variables.
    if (!(pNCvar->add_att("units",unit_name.c_str()))) return NC_ERROR;
    return 0;
  }
  //! add variables in NetCDF file
  /**
   * save variables in NetCDF file, may be called after creation of dimensions \c CImgNetCDF::addNetCDFDims
   *
   * \code
   *   dimv=2;char *fileName="2D2C.nc";
   *   C2DnCNetCDFvelocity vel;//this class MUST be derivated as dimi(), dimj() and dimv() are VIRTUAL
   *   vel.pNCFile=new NcFile(fileName,NcFile::Replace);
   *   vel.addNetCDFDims("dimX","dimY","time");
   *   std::vector<std::string> var_names(dimv+1);
   *   var_names[0]="vel_X";var_names[1]="vel_Y";
   *   var_names[2]="flag";
   *   std::vector<std::string> unit_names(2);
   *   unit_names[0]="m/s";unit_names[1]="none";
   *   addNetCDFVar(&var_names,&unit_names);
   * \endcode
   *
   * \see CImgNetCDF::addNetCDFDims
   *
  **/


//  int addNetCDFVar(CImg &img,std::string var_name,std::string unit_name,std::vector<std::string> dim_names,std::string time_name)
  int addNetCDFVar(CImg<T> &img,std::string var_name,std::string unit_name)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg<" << img.pixel_type() << ">," << var_name << "," << unit_name << ")" << std::endl;
#endif
    //check dims
	for(int a = 0 ; a<vpNCDim.size();a++){
		switch(a){
		case 3:if(loadDim(a)!=img.dimv()) {return DIM_ERROR;}break;
		case 2:if(loadDim(a)!=img.dimz()) {return DIM_ERROR;}break;
		case 1:if(loadDim(a)!=img.dimy()) {return DIM_ERROR;}break;
		case 0:if(loadDim(a)!=img.dimx()) {return DIM_ERROR;}break;	
		default:NULL;
		}
	}
    return addNetCDFVar(var_name,unit_name);
  }


  //! add variable data in NetCDF file
  /**
   * add variable data in NetCDF file, may be called after creation of both dimensions \c CImgNetCDF::addNetCDFDims and variables \c CImgNetCDF::addNetCDFVar 
   *
   * \code
   *   
   * \endcode
   *
   * \see CImgNetCDF::addNetCDFDims CImgNetCDF::addNetCDFVar
   *
  **/
/*  template<typename T> T* ptr(void)
  {
    return NULL;
  }*///###
  int addNetCDFData(void){
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl;
#endif
    if(pNCvar==NULL) return CODE_ERROR;
    int time=loadDimTime();
    T *ptr=NULL;//=(*this).ptr();
    if (!pNCvar->put_rec(ptr,time)) return NC_ERROR;
    return 0;
  }

  int addNetCDFData(CImg<T> &img,int time=-1){
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg<" << img.pixel_type() << ">,";
  std::cerr << time << " (if -1 append time))" << std::endl;
#endif
    	if(pNCvar==NULL) return CODE_ERROR;
	for(int a = 0 ; a<vpNCDim.size();a++){
		switch(a){
		case 3:if(loadDim(a)!=img.dimv()) {return DIM_ERROR;}break;
		case 2:if(loadDim(a)!=img.dimz()) {return DIM_ERROR;}break;
		case 1:if(loadDim(a)!=img.dimy()) {return DIM_ERROR;}break;
		case 0:if(loadDim(a)!=img.dimx()) {return DIM_ERROR;}break;	
		default:NULL;
		}
	}
    	//set time if needed (append mode)
    	if(time==-1) time=loadDimTime();
    	//write data
    	T *ptr=img.ptr();
    	if (!pNCvar->put_rec(ptr,time)) return NC_ERROR;
    	return 0;
  }



  //! load variable dimension from NetCDF file
  /**
   * load from NetCDF file a certain number of variables from names, that will set the variable dimension
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFVar(CImg<T> &img,std::string &var_name,std::string &pUnit_name){
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg,string,string)" << std::endl;
#endif
    ///get variable
    if (!(pNCvar=pNCFile->get_var(var_name.c_str()))) return NC_ERROR;
    ///set CImg dimensions if needed
	if(img.is_empty()){//std::cout<<"verif"<<vpNCDim.size()<<std::endl;
		switch(vpNCDim.size()){
			case 1:img.assign(loadDim(0));break;
			case 2:img.assign(loadDim(0),loadDim(1));break;
			case 3:img.assign(loadDim(0),loadDim(1),loadDim(2));break;
			case 4:img.assign(loadDim(0),loadDim(1),loadDim(2),loadDim(3));break;
			default:NULL;
		}
	}
    return 0;
  }
    int loadNetCDFVar(CImg<T> &img,std::string &var_name){
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg,string,string)" << std::endl;
#endif
    ///get variable
    if (!(pNCvar=pNCFile->get_var(var_name.c_str()))) return NC_ERROR;
    ///set CImg dimensions if needed
	if(img.is_empty()){//std::cout<<"verif"<<vpNCDim.size()<<std::endl;
		switch(vpNCDim.size()){
			case 1:img.assign(loadDim(0));break;
			case 2:img.assign(loadDim(0),loadDim(1));break;
			case 3:img.assign(loadDim(0),loadDim(1),loadDim(2));break;
			case 4:img.assign(loadDim(0),loadDim(1),loadDim(2),loadDim(3));break;
			default:NULL;
		}
	}
    return 0;
  }

  //! load dimensions from NetCDF file
  /**
   * load variable dimensions from NetCDF file
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFDims(std::vector<std::string> dim_names,std::string *pTime_name=NULL)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(";
  std::cerr << "," << ((pTime_name==NULL)?"default":(*pTime_name).c_str()) << ")" << std::endl;
#endif
    //check requested number of dimensions //###
    if(dim_names.size()!=4) std::cerr << "CImgNetCDF::" << __func__ << "(" << std::endl;
    NcDim*pd;
	for(int a = 0 ; a<dim_names.size();a++){
		if(!(pd=pNCFile->get_dim(dim_names[a].c_str())))return NC_ERROR;//Read
		else vpNCDim.push_back(pd);//Read
	}
    if(pTime_name!=NULL)
      if (!(pNCDimt=pNCFile->get_dim((*pTime_name).c_str())))        return NC_ERROR;//Read
    //dims can be now loaded using loadDim[x,y,z,v,Time]() methods
    return 0;
  }





//! load dimension names of a variable from NetCDF file
  /**
   * load variable names of a named variable from NetCDF file
   *
   * \param[in] var_name: name of the variable
   * \param[out] dim_names: names of variable dimensions (i.e. fixed dimensions)
   * \param[out] pTime_name: name of variable unlimited dimension (i.e. append dimension)
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFDimNames(std::string &var_name,std::vector<std::string> &dim_names,std::string *pTime_name=NULL)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(";
  std::cerr << "," << ((pTime_name==NULL)?"don't ":"") <<"search for unlimited dimension"<< ")" << std::endl<<std::flush;
#endif
    int i=0;
    NcDim*pNCDimtemp=NULL;
    
    ///get variable
    if (!(pNCvar=pNCFile->get_var(var_name.c_str()))) return NC_ERROR;
    //get dims
    
    if(pTime_name!=NULL){
      pTime_name->clear();
      if (!(pNCDimt=pNCvar->get_dim(i++)))return NC_ERROR;//Read
      *pTime_name=std::string(pNCDimt->name());
    }

    for(;i<pNCvar->num_dims();i++){
    	if (!(pNCDimtemp=pNCvar->get_dim(pNCvar->num_dims()-i))) return NC_ERROR;//Read
	vpNCDim.push_back(pNCDimtemp);
    }
    
    dim_names.clear();
    for(int a=0;a<vpNCDim.size();a++){//std::cout<<"ess "<<vpNCDim[a]->name()<<std::endl;
    	dim_names.push_back(std::string(vpNCDim[a]->name()));
    }
//#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << " information: dim_names=";
  for(std::vector<std::string>::iterator it=dim_names.begin();it!=dim_names.end();it++) std::cerr<<"\""<<*it<<"\", ";
  std::cerr << ((pTime_name==NULL)?"no unlimited dimension as been searched":"time_name=\""+*pTime_name+"\" (unlimited)") <<std::endl<<std::flush;
//#endif
    return 0;
  }
//(std::string)this <<



//! same as previously but without time
  /**
   * 
  **/
  int loadNetCDFDimNamesNoUnlimited(std::string &var_name,std::vector<std::string> &dim_names)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(";
#endif
    int i=1;
    NcDim*pNCDimtemp=NULL;
    
    ///get variable
    if (!(pNCvar=pNCFile->get_var(var_name.c_str()))) return NC_ERROR;
    //get dims
    for(;i<=pNCvar->num_dims();i++){
    	if (!(pNCDimtemp=pNCvar->get_dim(pNCvar->num_dims()-i))){
		return NC_ERROR;}//Read
	vpNCDim.push_back(pNCDimtemp);
    }
    
    dim_names.clear();
    for(int a=0;a<vpNCDim.size();a++){//std::cout<<"ess "<<vpNCDim[a]->name()<<std::endl;
    	dim_names.push_back(std::string(vpNCDim[a]->name()));
    }
//#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << " information: dim_names=";
  for(std::vector<std::string>::iterator it=dim_names.begin();it!=dim_names.end();it++) std::cerr<<"\""<<*it<<"\", ";
//#endif
    return 0;
  }



  //! check if NetCDF \c variable and \c CImg dims are the same
  /**
   * check if NetCDF \c variable and \c CImg dims are the same
   *
   * \arg img: \c CImg image dimensions to check
   *
   * \code
   *   CImg<int> img(1,2,3,4);CImgNetCDF<int> fp("data.nc");
   *   if( fp.checkDimx(img) || fp.checkDimy(img) || fp.checkDimz(img) || fp.checkDimv(img) ) exit(1);
   * \endcode
   *
   * \see DIM LOAD_DIM
   *
  **/
int checkDim(int d,CImg<T> img){
        std::cerr << "CImgNetCDF::" << __func__ << "(CImg)" << std::endl;
        if(d>=vpNCDim.size()) return CODE_ERROR;
	switch(d){
	case 0: return img.dimx()!=(int)vpNCDim[d]->size();break;
	case 1: return img.dimy()!=(int)vpNCDim[d]->size();break;
	case 2: return img.dimz()!=(int)vpNCDim[d]->size();break;
	case 3: return img.dimv()!=(int)vpNCDim[d]->size();break;
	default:NULL;
	}
}

#define CHECK_DIM(X,N) int checkDim##X(CImg<T> img) \
{ \
        std::cerr << "CImgNetCDF::" << __func__ << "(CImg)" << std::endl; \
        if(N>=vpNCDim.size()) return CODE_ERROR; \
        return img.dim##X()!=(int)vpNCDim[N]->size(); \
}
  CHECK_DIM(x,0)
  CHECK_DIM(y,1)
  CHECK_DIM(z,2)
  CHECK_DIM(v,3)


  //! load variable data from NetCDF file
  /**
   * load from NetCDF file variable data at a time
   *
   * \arg img: load data values in this \c CImg
   * \arg time: get data at time \c time (must be in the range 0 to < \c loadDimTime())
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFData(CImg<T> &img,int time=0)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg,time=" << time << ")" << std::endl;
#endif
    //check sizes
    int size=1;for(int d=0;d<vpNCDim.size();d++) size*=loadDim(d);
    ///memory allocation is same size
    if(img.size()==size)
    {
      img.width=loadDim(0);
      img.height=loadDim(1);
      img.depth=loadDim(2);
      img.dim=loadDim(3);
    }
    ///memory allocation is different size, so reallocates
    else
    {
      img.assign(loadDim(0),loadDim(1),loadDim(2),loadDim(3));
    }
    ///get component data
    if (!pNCvar->set_cur(time,0,0,0,0)) return NC_ERROR;
	switch(vpNCDim.size()){
		case 1:if (!pNCvar->get(img.ptr(0,0,0,0), 1,loadDim(0))) return NC_ERROR;break;
		case 2:if (!pNCvar->get(img.ptr(0,0,0,0), 1,loadDim(1),loadDim(0))) return NC_ERROR;break;
		case 3:if (!pNCvar->get(img.ptr(0,0,0,0), 1,loadDim(2),loadDim(1),loadDim(0))) return NC_ERROR;break;
		case 4:if (!pNCvar->get(img.ptr(0,0,0,0), 1,loadDim(3),loadDim(2),loadDim(1),loadDim(0))) return NC_ERROR;break;
		default:NULL;
	}
//TODO
    return 0;
  }




  //! 
  /**
   * 
  **/
int loadNetCDFDataNoUnlimited(CImg<T> &img)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg)" << std::endl;
#endif
	switch(vpNCDim.size()){
		case 1:if (!pNCvar->get(img.ptr(0,0,0,0), loadDim(0))) return NC_ERROR;break;
		case 2:if (!pNCvar->get(img.ptr(0,0,0,0), loadDim(1),loadDim(0))) return NC_ERROR;break;
		case 3:if (!pNCvar->get(img.ptr(0,0,0,0), loadDim(2),loadDim(1),loadDim(0))) return NC_ERROR;break;
		case 4:if (!pNCvar->get(img.ptr(0,0,0,0), loadDim(3),loadDim(2),loadDim(1),loadDim(0))) return NC_ERROR;break;
		default:NULL;
	}
    return 0;
  }

  int addNetCDFDims(std::vector<std::string> dim_names,std::string time_name)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(";
//  std::cerr << ((dim_names==NULL)?"default":dimi_name) << "," << ((dimj_name==NULL)?"default":dimj_name) << ",";
  std::cerr << time_name << ")" << std::endl;
#endif
	NcDim*pd;
	for(int a = 0 ; a<dim_names.size();a++){
		if(!(pd=pNCFile->add_dim(dim_names[a].c_str(),dim(a))))return NC_ERROR;//Read
		else vpNCDim.push_back(pd);
	}
    if(!(pNCDimt=pNCFile->add_dim(time_name.c_str()/*unlimited*/))) return NC_ERROR;//Write: add an unlimited dimension
    return 0;
  }
  int addNetCDFDims(CImg<T> &img,std::vector<std::string> dim_names,std::string time_name)
  {
  
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "(CImg<" << img.pixel_type() << ">,";
//  std::cerr << ((dim_names==NULL)?"default":dimi_name) << "," << ((dimj_name==NULL)?"default":dimj_name) << ",";
  std::cerr << time_name << ")" << std::endl;
#endif


	NcDim*pd;
	for(int a = 0 ; a<dim_names.size();a++){
		switch(a){
		case 0:if(!(pd=pNCFile->add_dim(dim_names[a].c_str(),img.dimx()))) return NC_ERROR;break;//Write
		case 1:if(!(pd=pNCFile->add_dim(dim_names[a].c_str(),img.dimy()))) return NC_ERROR;break;
		case 2:if(!(pd=pNCFile->add_dim(dim_names[a].c_str(),img.dimz()))) return NC_ERROR;break;
		case 3:if(!(pd=pNCFile->add_dim(dim_names[a].c_str(),img.dimv()))) return NC_ERROR;break;
		default:NULL;
		}
		vpNCDim.push_back(pd);
	}
    	if(!(pNCDimt=pNCFile->add_dim(time_name.c_str()/*unlimited*/))) return NC_ERROR;//Write: add an unlimited dimension
    	return 0;
  }


  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  int setNetCDFDims(std::vector<NcDim*> VDim,NcDim *pNetCDFDimt=NULL)
  {
#if cimg_debug>10
  	std::cerr << "CImgNetCDF::" << __func__ << "(" ;
	for(int a=0;a<VDim.size();a++){
		std::cerr << VDim[a]<< ",";
	std::cerr << pNetCDFDimt << ")" << std::endl;
#endif
	vpNCDim.clear();
    	for(int a=0;a<VDim.size();a++){
		vpNCDim.push_back(VDim[a]);
	}
    pNCDimt=pNetCDFDimt;
    return 0;
  }
};




//! 
/**
 * 
 *
 * \code
 *   
 * \endcode
 *
 * \see 
 *
**/

template<typename T> class CImgNetCDF_test:public CImgNetCDF<T>
{
  public:

#ifdef DIM
#undef DIM
#endif
#define DIM(X,N) int dim##X(void){\
	return (CImgNetCDF<T>::vpNCDim[N]);\
}

DIM(x,0)
DIM(y,1)
DIM(z,2)
DIM(v,3)


  //call C2DNetCDF::saveNetCDFFile
  int saveNetCDFFile(char *fileName,std::vector<std::string>dim_names,std::string time_name)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF_test::" << __func__ << "(" << fileName << "," ;
  std::cerr << time_name << ")" << std::endl;
#endif
    return CImgNetCDF<T>::saveNetCDFFile(fileName,&dim_names,&time_name);
  }




  //! add variable data in NetCDF file
  /**
   * add variable data in NetCDF file, may be called after creation of both dimensions \c CImgNetCDF::addNetCDFDims and variables \c CImgNetCDF::addNetCDFVar 
   *
   * \code
   *   
   * \endcode
   *
   * \see CImgNetCDF::addNetCDFDims CImgNetCDF::addNetCDFVar
   *
  */
  int addNetCDFData(void){
#if cimg_debug>10
  std::cerr << "CImgNetCDF::" << __func__ << "()" << std::endl;
#endif
    if((*this).pNCvar==NULL) return CODE_ERROR;
    int time=CImgNetCDF<T>::loadDimTime();//vpNCDim[2] << "," << vpNCDim[3]
    //make dummy data
	int dim=1;
	for(int i=0 ; i < (CImgNetCDF<T>::vpNCDim.size());i++)
    		dim=dim*(CImgNetCDF<T>::vpNCDim[i]);
    T *ptr=(T*)malloc(sizeof(T)*dim);
    for(int i=0;i<dim;i++) ptr[i]=(T)i;
    //write
    if (!((*this).pNCvar)->put_rec(ptr,time)) return NC_ERROR;
    return 0;
  }





  //call C2DNetCDF::loadFile
  int loadFile(char *fileName,char *dimi_name=NULL,char *dimj_name=NULL,char *time_name=NULL)
  {
#if cimg_debug>10
  std::cerr << "CImgNetCDF_test::" << __func__ << "(" << fileName << "," << ((dimi_name==NULL)?"default":dimi_name) << "," << ((dimj_name==NULL)?"default":dimj_name) << "," << ((time_name==NULL)?"default":time_name) << ")" << std::endl;
#endif
    CImgNetCDF<T>::loadFile(fileName,dimi_name,dimj_name,time_name);
    //then may use loadDimi(),loadDimj(),loadDimTime() to allocate
  }
  //add function
  void print(void){
#if cimg_debug>10
  std::cerr << "CImgNetCDF_test::" << __func__ << "()" << std::endl;
#endif
    if(CImgNetCDF<T>::vpNCDim.size()>=1)std::cout<<"dimx="<<dimx()<<",";
    if(CImgNetCDF<T>::vpNCDim.size()>=2)std::cout<<"dimy="<<dimy()<<",";
    if(CImgNetCDF<T>::vpNCDim.size()>=3)std::cout<<"dimz="<<dimz()<<",";
    if(CImgNetCDF<T>::vpNCDim.size()==4)std::cout<<"dimv="<<dimv()<<",";
    std::cout<<"dimt="<<CImgNetCDF<T>::loadDimTime();
	
  }
};





//! add NetCDF read/write support to CImg class
/**
 * an object of this class may be created or CImg may derived from this class to handle NetCDF
 * \li object in \c .cpp
 * \code
 *   CImgNetCDF<int> object;
 * \endcode
 * \li derivation in \c .h
 * \code
 *   template<typename T> CnewClass: \c CImg<T>, \c CImgNetCDF<T>
 * \endcode
 *
 * \note \c CImgNetCDF and \c CImg must be of the same type
 * \note For this implementation, NetCDF variable dimensions need to be 5: four fixed dimensions (x,y,z,v) and one unlimited dimension (t)
 *
 * \code
 *   
 * \endcode
 *
 * \see CImg
 *
**/
template<typename T> class CImgListNetCDF: public CImgNetCDF<T>
{
//variable
  public:
  //! pointers to NetCDF data
  /**
   * access to NetCDF data
   *
   * \code
   *   
   * \endcode
   *
  **/
  std::vector<NcVar*> pNCvars;
  //! 
  /**
   * 
   *
   * \param  : 
   *
   * \code
   *   
   * \endcode
   *
  **/
  int addNetCDFDims(CImgList<T> &imgs,std::vector<std::string> dim_names,std::string time_name)
  {
#if cimg_debug>10
  std::cerr << "CImgListNetCDF::" << __func__ << "(CImgList<" << imgs.pixel_type() << ">(" << imgs.size << "),";
//  std::cerr << ((dim_names==NULL)?"default":dimi_name) << "," << ((dimj_name==NULL)?"default":dimj_name) << ",";
  std::cerr << time_name << ")" << std::endl;
#endif
    //check sizes
    if(imgs.size<1) {return DIM_ERROR;}
    return CImgNetCDF<T>::addNetCDFDims(imgs(0),dim_names,time_name);
  }
  //! add variables in NetCDF file
  /**
   * save variables in NetCDF file, may be called after creation of dimensions \c CImgNetCDF::addNetCDFDims
   *
   * \code
   *   dimv=2;char *fileName="2D2C.nc";
   *   C2DnCNetCDFvelocity vel;//this class MUST be derivated as dimi(), dimj() and dimv() are VIRTUAL
   *   vel.pNCFile=new NcFile(fileName,NcFile::Replace);
   *   vel.addNetCDFDims("dimX","dimY","time");
   *   std::vector<std::string> var_names(dimv+1);
   *   var_names[0]="vel_X";var_names[1]="vel_Y";
   *   var_names[2]="flag";
   *   std::vector<std::string> unit_names(2);
   *   unit_names[0]="m/s";unit_names[1]="none";
   *   addNetCDFVar(&var_names,&unit_names);
   * \endcodeif((*this).loadDim(0)!=imgs(0).dimx()) {return DIM_ERROR;}
   *
   * \see CImgNetCDF::addNetCDFDims
   *
  **/
  int addNetCDFVar(CImgList<T> &imgs,std::vector<std::string> var_names,std::vector<std::string> unit_names)
  {
#if cimg_debug>10
  std::cerr << "CImgListNetCDF::" << __func__ << "(CImgList<" << imgs.pixel_type() << ">(" << imgs.size << "),";
// << var_names << "," << unit_names
   std::cerr << ")" << std::endl;
#endif
    //check sizes
    if(imgs.size<1) {return DIM_ERROR;}
    if(var_names.size()!=unit_names.size()) {return DIM_ERROR;}
    //check dims
	for(int a=0 ; a<CImgNetCDF<T>::vpNCDim.size() ; a++){
		switch(a){
		case 0:if((*this).loadDim(a)!=imgs(0).dimx()) {return DIM_ERROR;}break;
		case 1:if((*this).loadDim(a)!=imgs(0).dimy()) {return DIM_ERROR;}break;
		case 2:if((*this).loadDim(a)!=imgs(0).dimz()) {return DIM_ERROR;}break;
		case 3:if((*this).loadDim(a)!=imgs(0).dimv()) {return DIM_ERROR;}break;
		}
	}
	
    //allocates
    pNCvars.assign(imgs.size,NULL);
    //var component
    for(int i=0;i<pNCvars.size();i++)
    {
      if((*this).CImgNetCDF<T>::addNetCDFVar(var_names[i],unit_names[i])) {return NC_ERROR;}
      pNCvars[i]=(*this).pNCvar;(*this).pNCvar=NULL;
    }
    return 0;
  }




  //! add variable data in NetCDF file
  /**
   * add variable data in NetCDF file, may be called after creation of both dimensions \c CImgNetCDF::addNetCDFDims and variables \c CImgNetCDF::addNetCDFVar 
   *
   * \code
   *   
   * \endcode
   *
   * \see CImgListNetCDF::addNetCDFDims CImgListNetCDF::addNetCDFVar
   *
  **/
  int addNetCDFData(CImgList<T> &imgs,int time=-1)
  {
#if cimg_debug>10
  std::cerr << "CImgListNetCDF::" << __func__ << "(CImgList<" << imgs.pixel_type() << ">(" << imgs.size << ")";
  std::cerr << ",time=" << time << " (if -1 append time))" << std::endl;
#endif
    if(pNCvars.empty()) return CODE_ERROR;
    //check sizes
    if(imgs.size!=pNCvars.size()) {return DIM_ERROR;}
    //set time if needed (append mode)
    if(time==-1) time=(*this).CImgNetCDF<T>::loadDimTime();
    //var component
    for(int i=0;i<pNCvars.size();i++)
    {
      //write data
      (*this).pNCvar=pNCvars[i];//set current variable data pointer
      if((*this).CImgNetCDF<T>::addNetCDFData(imgs[i],time)) {return NC_ERROR;}
      (*this).pNCvar=NULL;
    }
    return 0;
  }






  //! load variable dimension from NetCDF file
  /**
   * load from NetCDF file a certain number of variables from names, that will set the variable dimension
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFVar(CImgList<T> &imgs,std::vector<std::string> &var_names,std::vector<std::string> &unit_names)
  {
#if cimg_debug>10
  std::cerr << "CImgListNetCDF::" << __func__ << "(CImgList<" << imgs.pixel_type() << ">(" << imgs.size << ")";
  std::cerr << ",vector<string>,vector<string>)" << std::endl;
#endif
    //allocates
    pNCvars.assign(var_names.size(),NULL);
    //allocates if needed
    if(imgs.is_empty()) {
	switch(CImgNetCDF<T>::vpNCDim.size()){
		case 4:imgs.assign(pNCvars.size(),(*this).loadDim(0),(*this).loadDim(1),(*this).loadDim(2),(*this).loadDim(3));break;
		case 3:imgs.assign(pNCvars.size(),(*this).loadDim(0),(*this).loadDim(1),(*this).loadDim(2));break;
		case 2:imgs.assign(pNCvars.size(),(*this).loadDim(0),(*this).loadDim(1));break;
		case 1:imgs.assign(pNCvars.size(),(*this).loadDim(0));break;
		default:NULL;
	}
	}
    //var component
    for(int i=0;i<pNCvars.size();i++)
    {
      //load variable and allocates if needed
      if((*this).CImgNetCDF<T>::loadNetCDFVar(imgs[i],var_names[i],unit_names[i])) {return NC_ERROR;}//### pUnit_names
      pNCvars[i]=(*this).pNCvar;//set current variable data pointer
      (*this).pNCvar=NULL;
    }
    return 0;
  }


  //! load variable dimension from NetCDF file
  /**
   * load from NetCDF file a certain number of variables from names, that will set the variable dimension
   *
   * \code
   *   
   * \endcode
   *
  **//*
template <typename gridT*>  int loadNetCDFVar(vector<CImg<gridT> > &grid,std::vector<std::string> &grid_names,std::vector<std::string> &grid_unit)
  {
#if cimg_debug>10
  std::cerr << "CImgListNetCDF::" << __func__ << "(CImgList<" << grid.pixel_type() << ">(" << grid.size << ")";
  std::cerr << ",vector<string>,vector<string>)" << std::endl;
#endif
    //allocates
    pNCvars.assign(grid_names.size(),NULL);
    //allocates if needed
    if(grid.empty()) {return 0;}
    //var component
    for(int i=0;i<pNCvars.size();i++)
    {
      //load variable and allocates if needed
      if((*this).CImgNetCDF<T>::loadNetCDFVar((*grid[i]),grid_names[i],grid_unit[i])) {return NC_ERROR;}//### pUnit_names
      pNCvars[i]=(*this).pNCvar;//set current variable data pointer
      (*this).pNCvar=NULL;
    }
    return 0;
  }*/



  //! load variable data from NetCDF file
  /**
   * load from NetCDF file variable data at a time
   *
   * \arg img: load data values in this \c CImg
   * \arg time: get data at time \c time (must be in the range 0 to < \c loadDimTime())
   *
   * \code
   *   
   * \endcode
   *
  **/
  int loadNetCDFData(CImgList<T> &imgs,int time=0)
  {
#if cimg_debug>10
  std::cerr << "CImgListNetCDF::" << __func__ << "(CImgList<" << imgs.pixel_type() << ">(" << imgs.size << ")";
  std::cerr << ",time=" << time << ")" << std::endl;
#endif
    if(pNCvars.empty()) return CODE_ERROR;
    //check sizes
    if(imgs.size!=pNCvars.size()) {return DIM_ERROR;}
    //var component
    
    for(int i=0;i<pNCvars.size();i++)
    {
      //write data
      (*this).pNCvar=pNCvars[i];//set current variable data pointer
      if((*this).CImgNetCDF<T>::loadNetCDFData(imgs[i],time)) {return NC_ERROR;}
      (*this).pNCvar=NULL;
    }
    return 0;
  }
};

#endif/*CIMG_NETCDF*/

