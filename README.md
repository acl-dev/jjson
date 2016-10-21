# jjson
mini json,one header file. easy to use. 

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

	str = json2.to_string();
  
	  this json:
	  {
	    "obj1": {
		"obj1": 1,
		"obj2": false,
		"obj3": 0.1,
		"obj4": [
		    1,
		    false
		],
		"obj5": {
		    "ob1": false,
		    "ob2": 1,
		    "ob3": {
			"bool": false,
			"float": 0.1,
			"int": 12345,
			"str": "中国人",
			"vec": [
			    1,
			    2,
			    3,
			    4,
			    "hello world",
			    false,
			    true,
			    null,
			    {
				"bool": false,
				"str": "中国人"
			    },
			    [
				1,
				false
			    ]
			],
			"vec1": [
			    1,
			    false
			]
		    }
		}
	    },
	    "vec": {
		"bool": false,
		"float": 0.1,
		"int": 12345,
		"str": "中国人",
		"vec": [
		    1,
		    2,
		    3,
		    4,
		    "hello world",
		    false,
		    true,
		    null,
		    {
			"bool": false,
			"str": "中国人"
		    },
		    [
			1,
			false
		    ]
		],
		"vec1": [
		    1,
		    false
		]
	    }
	}
