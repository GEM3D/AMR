//#include "/home/jhasbestan/GEM_AMR/src/include/parse_stl.h"
//#include "/home/jhasbestan/GEM_AMR/src/include/typedefs.h"

#include "parse_stl.h"
#include "typedefs.h"




namespace stl {
  std::ostream& operator<<(std::ostream& out, const point p) {
    out << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;
    return out;
  }

  std::ostream& operator<<(std::ostream& out, const triangle& t) {
    out << "---- TRIANGLE ----" << std::endl;
    out << t.normal << std::endl;
    out << t.v1 << std::endl;
    out << t.v2 << std::endl;
    out << t.v3 << std::endl;
    return out;
  }
  
  float parse_float(std::ifstream& s) {
    char f_buf[sizeof(float)];
    s.read(f_buf, 4);
    float* fptr = (float*) f_buf;
    return *fptr;
  }

  point parse_point(std::ifstream& s) {
    float x = parse_float(s);
    float y = parse_float(s);
    float z = parse_float(s);
    return point(x, y, z);
  }

  stl_data parse_stl(const std::string& stl_path) {
    std::ifstream stl_file(stl_path.c_str(), std::ios::in | std::ios::binary);
    if (!stl_file) {
      std::cout << "ERROR: COULD NOT READ FILE" << std::endl;
      assert(false);
    }

    char header_info[80] = "";
    char n_triangles[4];
    stl_file.read(header_info, 80);
    stl_file.read(n_triangles, 4);
    std::string h(header_info);
    stl_data info(h);
    unsigned int* r = (unsigned int*) n_triangles;
    unsigned int num_triangles = *r;
    for (unsigned int i = 0; i < num_triangles; i++) {
      auto normal = parse_point(stl_file);
      auto v1 = parse_point(stl_file);
      auto v2 = parse_point(stl_file);
      auto v3 = parse_point(stl_file);
      info.triangles.push_back(triangle(normal, v1, v2, v3));
      char dummy[2];
      stl_file.read(dummy, 2);
    }
    return info;
  }

}
/*
int main(int argc, char* argv[]) {
  assert(argc == 2);
  auto info=stl::parse_stl(argv[1]);
  std::vector<stl::triangle> triangles = info.triangles;
  std::cout << "STL HEADER = " << info.name << std::endl;
  std::cout << "# triangles = " << triangles.size() << std::endl;
  
  // This is for debugging
  std::cout << info.triangles[88000]<<std::endl;
  
  for (auto t : info.triangles) {
    std::cout << t << std::endl;
  }

}
*/
//======================================================================
//======================================================================
//======================================================================

void ReadSTLGeom(int argc, char* argv[],double **triangle_center, int *nn,const double *xyz)
{
  double scale=0.02;	
  double disp=1.0;
  if(argc != 2)
  {
	  printf(ANSI_COLOR_RED "Geometry Not Supplied\n" ANSI_COLOR_RESET);
  }
  auto info=stl::parse_stl(argv[1]);
  std::vector<stl::triangle> triangles = info.triangles;
  std::cout << "STL HEADER = " << info.name << std::endl;
//  std::cout << "# triangles = " << triangles.size() << std::endl;

   double center[3]={0.0,0.0,0.0};
	  
  // This is for debugging
  //std::cout << info.triangles[88000]<<std::endl;
  std::cout<<triangles.size()<<std::endl;
  
  // distribute the geometry between the blocks looking at their block extent (or ancestor length)
    
  for(unsigned int i=0;i<triangles.size();i++)
	{
		center[0]+=(info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x);
		center[1]+=(info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y);
		center[2]+=(info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z);
	}
  
  	    center[0]=center[0]/triangles.size()/3.0;
		center[1]=center[1]/triangles.size()/3.0;
		center[2]=center[2]/triangles.size()/3.0;
  
  // min and max can be found and then converted to center by division by 3
  
    double xmax=(info.triangles[0].v1.x+info.triangles[0].v2.x+info.triangles[0].v3.x);
	double ymax=(info.triangles[0].v1.y+info.triangles[0].v2.y+info.triangles[0].v3.y);
	double zmax=(info.triangles[0].v1.z+info.triangles[0].v2.z+info.triangles[0].v3.z);
  
  for(unsigned int i=0;i<triangles.size();i++)
	{
		if(info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x>xmax)
		{
			xmax=info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x;
		}
		
				
		if(info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y>ymax)
		{
			ymax=info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y;
		}
		if(info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z>zmax)
		{
			zmax=info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z;
		}		
	}
	
	scale=xmax;
	if(ymax>scale)
	{
		scale=ymax;
	}
	if(zmax>scale)
	{
		scale=zmax;
	}
	scale=3./scale;
  
  
  for(unsigned int i=0;i<triangles.size();i++)
	{
		info.triangles[i].v1.x=info.triangles[i].v1.x*scale-center[0]*scale;
		info.triangles[i].v1.y=info.triangles[i].v1.y*scale-center[1]*scale;
		info.triangles[i].v1.z=info.triangles[i].v1.z*scale-center[2]*scale;
		
		info.triangles[i].v2.x=info.triangles[i].v2.x*scale-center[0]*scale;
		info.triangles[i].v2.y=info.triangles[i].v2.y*scale-center[1]*scale;
		info.triangles[i].v2.z=info.triangles[i].v2.z*scale-center[2]*scale;
		
		info.triangles[i].v3.x=info.triangles[i].v3.x*scale-center[0]*scale;
		info.triangles[i].v3.y=info.triangles[i].v3.y*scale-center[1]*scale;
		info.triangles[i].v3.z=info.triangles[i].v3.z*scale-center[2]*scale;
		
		
	}
	
	
	*nn=0;
	int bol[3]={0,0,0};
	
	for(unsigned int i=0;i<triangles.size();i++)
	{
		
				
		bol[0]=(info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x)<= xyz[1]*3. && (info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x)>= xyz[0]*3.;
		bol[1]=(info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y)<= xyz[3]*3. && (info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y)>= xyz[2]*3.;
		bol[2]=(info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z)<= xyz[5]*3. && (info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z)>= xyz[4]*3;
		
		if(bol[0] && bol[1] && bol[2])
		{
		(*nn)++;
	    }
	}
	printf("nn=%d\n",*nn);
  

   *triangle_center=(double*)malloc(3*(*nn)*sizeof(double));
   
   int count=0;
   
   for(unsigned int i=0;i<triangles.size();i++)
	{
		
		bol[0]=(info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x)<= xyz[1]*3. && (info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x)>= xyz[0]*3.;
		bol[1]=(info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y)<= xyz[3]*3. && (info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y)>= xyz[2]*3.;
		bol[2]=(info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z)<= xyz[5]*3. && (info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z)>= xyz[4]*3;
	
		
		if(bol[0] && bol[1] && bol[2])
		{
		(*triangle_center)[3*count  ]=(info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x)/3.0;
		(*triangle_center)[3*count+1]=(info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y)/3.0;
		(*triangle_center)[3*count+2]=(info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z)/3.0;
		count++;
	    }
	}
   
   
  /*	
	for(unsigned int i=0;i<triangles.size();i++)
	{
		(*triangle_center)[3*i  ]=(info.triangles[i].v1.x+info.triangles[i].v2.x+info.triangles[i].v3.x)/3.0;
		(*triangle_center)[3*i+1]=(info.triangles[i].v1.y+info.triangles[i].v2.y+info.triangles[i].v3.y)/3.0;
		(*triangle_center)[3*i+2]=(info.triangles[i].v1.z+info.triangles[i].v2.z+info.triangles[i].v3.z)/3.0;
		
		center[0]=(*triangle_center)[3*i]+center[0];
		center[1]=(*triangle_center)[3*i+1]+center[1];
		center[2]=(*triangle_center)[3*i+2]+center[2];
		/*
		std::cout<<(*triangle_center)[3*i]<<std::endl;
		std::cout<<(*triangle_center)[3*i+1]<<std::endl;
		std::cout<<(*triangle_center)[3*i+2]<<std::endl;
		
	}
	
	// normalize the size 
	double xmax=(*triangle_center)[0];
	double ymax=(*triangle_center)[1];
	double zmax=(*triangle_center)[2];
	
	
	
	for(int i=0;i<(*nn);i++)
	{
		
		if((*triangle_center)[3*i]>xmax)
		{
			xmax=(*triangle_center)[3*i];
		}
		if((*triangle_center)[3*i+1]>xmax)
		{
			ymax=(*triangle_center)[3*i+1];
		}
		if((*triangle_center)[3*i+2]>xmax)
		{
			zmax=(*triangle_center)[3*i+2];
		}		
	}
	
	scale=xmax;
	if(ymax>scale)
	{
		scale=ymax;
	}
	if(zmax>scale)
	{
		scale=zmax;
	}
	scale=1./scale;
	
	
	for(unsigned int i=0;i<triangles.size();i++)
	{
		(*triangle_center)[3*i  ]=(*triangle_center)[3*i  ]*scale-center[0]*scale;
		(*triangle_center)[3*i+1]=(*triangle_center)[3*i+1]*scale-center[1]*scale;
		(*triangle_center)[3*i+2]=(*triangle_center)[3*i+2]*scale-center[2]*scale;
		/*
		std::cout<<(*triangle_center)[3*i]<<std::endl;
		std::cout<<(*triangle_center)[3*i+1]<<std::endl;
		std::cout<<(*triangle_center)[3*i+2]<<std::endl;
		
	}
	*/
	
	printf("scale=%16.16lf %16.16lf %16.16lf %16.16lf \n",scale,center[0]*scale,center[1]*scale,center[2]*scale);
		
}





