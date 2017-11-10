#ifndef _UIBCORBA_OBJECT_H_
#define _UIBCORBA_OBJECT_H_

#if defined(WIN32)

#if defined(ORBCOM_CREATE_DLL)
#define	ORBCOM_DLL_IMEXPORT	__declspec(dllexport)
#else	//_UIB_BASE_
#define ORBCOM_DLL_IMEXPORT	__declspec(dllimport)
#endif	//_UIB_BASE_

#else		//UNIX
#define	ORBCOM_DLL_IMEXPORT
#endif		//UNIX

#include <cassert>
#include <string>
#include <tao/ORB.h>
#include <orbsvcs/Naming/Naming_Client.h>

namespace UIBCORBA_Object {
	static int s_orb_id = 0;
	//用于获得能够访问IP:PORT指定服务的ORB环境，返回0表示错误
	ORBCOM_DLL_IMEXPORT CORBA::ORB_ptr InitializeORB(
												int			in_UseNamingService,
												int			in_MultiCast,
												const char* in_ip,
												const char* in_port,
												const char* in_orb_id = 0	);
	//通过SetORB()得到的ORB环境，获得指定的服务对象引用，返回0表示错误
	template <typename ObjectType>
	ObjectType* GetObj(	CORBA::ORB_ptr in_orb,
						int			in_UseNamingService,
						const char* in_serv_name	);

	//销毁客户ORB
	ORBCOM_DLL_IMEXPORT void DestroyORB(CORBA::ORB_ptr in_orb);
};

namespace UIBCORBA_Object {
	template <typename ObjectType>
	ObjectType* GetObj(	CORBA::ORB_ptr in_orb,
						int			in_UseNamingService,
						const char* in_serv_name	) {
		assert(in_orb);
		assert(in_serv_name);
		
		ObjectType* rtn_obj;
		CORBA::Object_var t_obj;	//wfp changed at 20100903 from Object_ptr to var(智能指针)

		if(in_UseNamingService){
			try {
				// Find the Naming Service
				TAO_Naming_Client naming_client;

				if (naming_client.init (in_orb/*.in ()*/) != 0) {
					//cerr << "Could not initialize naming client." << endl;
					return 0;
				}

				// Resolve the Messenger object
				CosNaming::Name name;
				name.length( 2 );
				name[0].id = CORBA::string_dup( "econiisp" );
				name[1].id = CORBA::string_dup( in_serv_name );
				/*CORBA::Object_ptr*/ t_obj = naming_client->resolve(name);

				// Narrow
				rtn_obj = ObjectType::_narrow(t_obj/*.in()*/);
				if (CORBA::is_nil(rtn_obj/*.in()*/)){
					//cerr << "Not a mdserver reference" << endl;
					return 0;
				}

				return rtn_obj;
			}

			catch ( CORBA::Exception& ) {
				//cerr << ex._rep_id() << endl;
				//cerr << ex._name() << endl;
				return 0;
			}
		}
		else{
			try{
				/*CORBA::Object_ptr t_obj;*/
				t_obj = in_orb->resolve_initial_references(in_serv_name);
				rtn_obj = ObjectType::_narrow(t_obj);
				if(CORBA::is_nil(rtn_obj/*.in()*/)){
					return 0;
				}

				return rtn_obj;
			}
			catch (CORBA::Exception&) {
				//cerr << ex._rep_id() << endl;
				//cerr << ex._name() << endl;
				return 0;
			}
		}
	}
}

#endif
