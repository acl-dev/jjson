#include "jjson.hpp"
#include <thread>
jjson::json_t json1, json2;
std::string str;

int test_parse()
{
	jjson::json_t *json = jjson::build_json(str);
	delete json;
	return 0;
}
void test_get()
{
	assert(json1["str"].get<std::string>() == "中国人");
	assert(json1["int"].get<int>() == 12345);
	assert(json1["bool"].get<bool>() == false);
	assert(json1["float"].type() == jjson::e_float);
	assert(json1["vec"].get<std::string>(4) == "hello world");
	assert(json1["vec"].get<bool>(5) == false);
	assert(json1["vec"].get<bool>(6) == true);
	assert(json1["vec"].get(7).is_null());
	assert(json1["vec"].get(8)["str"].get<std::string>() == "中国人");
	assert(json1["vec"].get(8)["bool"].get<bool>() == false);
	assert(json2["obj1"]["obj5"]["ob2"].get<int>() == 1);

}
void test_init()
{
	jjson::obj_t vec;
	jjson::json_t obj;
	obj["str"] = "中国人";
	obj["bool"] = false;

	vec.add(1);
	vec.add(false);


	json1["str"] = "中国人";
	json1["int"] = 12345;
	json1["bool"] = false;
	json1["float"] = 0.1;

	json1["vec1"] = vec;
	json1["vec"].add(1);
	json1["vec"].add(2);
	json1["vec"].add(3);
	json1["vec"].add(4);
	json1["vec"].add("hello world");
	json1["vec"].add(false);
	json1["vec"].add(true);
	json1["vec"].add(jjson::null);

	json1["vec"].add(obj);
	json1["vec"].add(vec);

	json2["vec"] = json1;
	json2["obj1"]["obj1"] = 1;
	json2["obj1"]["obj2"] = false;
	json2["obj1"]["obj3"] = 0.1;
	json2["obj1"]["obj4"] = vec;
	json2["obj1"]["obj5"]["ob1"] = false;
	json2["obj1"]["obj5"]["ob2"] = 1;
	json2["obj1"]["obj5"]["ob3"] = json1;

	str = json1.to_string();
}
int main()
{
	test_init();
	//test_get();
	int64_t i = 0;
	int64_t lasti = 0;
	int len = str.size();
	std::thread timer([&] { 
		while(true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << ((i - lasti) * len)/1024/1024 <<" MB"<<std::endl;
			lasti = i;
		}
	});
	while(true)
	{
		i++;
		test_get();
	}
}