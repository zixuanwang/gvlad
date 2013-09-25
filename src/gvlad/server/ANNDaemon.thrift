namespace cpp gvlad
namespace php gvlad

struct Neighbor{
	1:string id,
	2:double distance
}

service ANNDaemon{
	list<Neighbor> query(1:string image_path, 2:i32 k)
}
