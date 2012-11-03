#ifndef __ODBCMANAGER_H__
#define __ODBCMANAGER_H__

#include "Prerequisites.h"
#include "Singleton.h"
#include "CDataList.h"
#include <sql.h>
#include <sqlext.h>

namespace UOX
{
	class ODBCManager : public Singleton< ODBCManager >
	{
	protected:
		SQLHENV					henv;                     // ODBC Environment Handle
		SQLHDBC					hdbc;                     // ODBC Connection Handle
		SQLRETURN				rc;
		std::vector< SQLHSTMT > statementList;
		int						allocationState;
		bool					inTransaction;
		bool					lastState;
		std::string				database;
		std::string				username;
		std::string				password;

		bool AllocateHandles( void );
		void DeallocateHandles( void );

		void DisplayError( int handleType, SQLHANDLE toDiag );

	public:
		ODBCManager();
		~ODBCManager();

		bool SetDatabase( std::string newVal );
		bool SetUsername( std::string newVal );
		bool SetPassword( std::string newVal );

		bool Connect( void );
		bool Disconnect( void );
		bool ExecuteQuery( std::string sql, int *index = NULL );
		bool FetchRow( int index );
		bool GetColumn( int colNumber, UString& value, int index );
		bool QueryRelease( void );
		bool SaveSettings( std::ofstream& target );
		bool BeginTransaction( void );
		bool FinaliseTransaction( bool commit );
		bool LastSucceeded( void );

        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static ODBCManager& getSingleton( void );
        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static ODBCManager * getSingletonPtr( void );
	};
}

#endif