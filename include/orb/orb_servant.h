#ifndef _ENDPOINT_SERVANT_H_
#define _ENDPOINT_SERVANT_H_

#if defined(WIN32)

#if defined(ORBCOM_CREATE_DLL)
#define	ORBCOM_DLL_IMEXPORT	__declspec(dllexport)
#else	//_UIB_BASE_
#define ORBCOM_DLL_IMEXPORT	__declspec(dllimport)
#endif	//_UIB_BASE_

#else		//WIN32
#define	ORBCOM_DLL_IMEXPORT
#endif		//WIN32

#include <cassert>
#include <string>
#include <tao/ORB.h>
//#include <tao/PortableServer/poa.h>
#include <tao/PortableServer/PortableServer.h>
//#include <tao/RTPortableServer/RT_POA.h>
#include <tao/IORTable/IORTable.h>
#include <orbsvcs/CosNamingC.h>


namespace UIBCORBA_Servant {
	static int s_orb_id = 0;
	struct ORB_Env {
		CORBA::ORB_ptr					orb_ref;
		PortableServer::POA_ptr			poa_ref;
		IORTable::Table_ptr				ior_table_ref;
		CosNaming::NamingContext_ptr	context_root_ref;
	};
	//用于获得能够发布指定IP:PORT的服务的ORB环境，返回0表示成功，<0表示失败，注意不要和其他ORB环境冲突
	ORBCOM_DLL_IMEXPORT int InitializeORB(	int		in_UseNamingService,
								int				in_MultiCast,
								const char*		in_ip,
								const char*		in_port,
								ORB_Env&		out_env,
								const char*		in_orb_id = 0	);
	//通过给定ORB环境，获得指定的服务对象引用，返回0表示成功，<0表示失败
	template <typename ServantType>
	int ActivateObj(	const ORB_Env& in_env,
									int			in_UseNamingService,
									const PortableServer::Servant in_servant_ref,
									const char* in_serv_name	);
	ORBCOM_DLL_IMEXPORT void RunORB(const ORB_Env& in_env);
	ORBCOM_DLL_IMEXPORT void ShutdownORB(const ORB_Env& in_env);
};

namespace UIBCORBA_Servant{
	template <typename ServantType>
	int ActivateObj(const ORB_Env& in_env, 
					int				in_UseNamingService,
					ServantType*	in_servant_ref,
					const char*		in_serv_name ){

		assert(in_env.orb_ref);
		assert(in_env.poa_ref);
		assert(in_servant_ref);
		assert(in_serv_name);

		CORBA::Object_var obj;
		CORBA::String_var ior_string;
		PortableServer::ObjectId_var obj_id;

		try {
			if(in_UseNamingService){
				if(!in_env.context_root_ref)return -1;

				CosNaming::Name name;
				name.length(2);
				name[0].id = CORBA::string_dup("econiisp");
				name[1].id = CORBA::string_dup( in_serv_name );

				PortableServer::ObjectId_var obj_id = in_env.poa_ref->activate_object(in_servant_ref);
				obj = in_env.poa_ref->id_to_reference(obj_id.in());
				// add for ior file wlinq [6/26/2007]
				ior_string = in_env.orb_ref->object_to_string(obj.in());
				// add end[6/26/2007]
				in_env.context_root_ref->rebind(name,obj.in());
			}
			else{
				assert(in_env.ior_table_ref);

				obj_id = in_env.poa_ref->activate_object(in_servant_ref);
				obj = in_env.poa_ref->id_to_reference(obj_id.in());
				ior_string = in_env.orb_ref->object_to_string(obj.in());
				in_env.ior_table_ref->bind(in_serv_name, ior_string.in());
			}
			// add for ior file wlinq [6/26/2007]
			//save_ior_to_file(ior_string,in_serv_name);
			FILE *fp;

			char	filename[128];
			if(in_serv_name == NULL) return 1;

			ACE_OS::strcpy(filename,"C:\\");

			ACE_OS::strcat(filename,in_serv_name);
			ACE_OS::strcat(filename,".ior");

			fp=ACE_OS::fopen(filename,"w");
			if(fp==NULL) return 1;

			ACE_OS::fprintf(fp,"%s",(char*)ior_string); 

			ACE_OS::fclose(fp);
			// add end [6/26/2007]
		}
		catch (...) {
			return -1;
		}
		return 1;
	}
}

#endif
