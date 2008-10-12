#include "ODBCManager.h"
#include "uox3.h"

namespace UOX
{
	const int ALLOC_NONE	= 0;
	const int ALLOC_ENV		= 1;
	const int ALLOC_DBC		= 2;
	const int ALLOC_CONNECT = 3;
	const int ALLOC_STMT	= 4;

    //---------------------------------------------------------------------------------------------
    template<> ODBCManager * Singleton< ODBCManager >::ms_Singleton = 0;
    ODBCManager* ODBCManager::getSingletonPtr( void )
    {
        return ms_Singleton;
    }
    ODBCManager& ODBCManager::getSingleton( void )
    {  
        assert( ms_Singleton );  return ( *ms_Singleton );  
    }
    //---------------------------------------------------------------------------------------------

	ODBCManager::ODBCManager() : allocationState( 0 ), inTransaction( false )
	{
	}
	ODBCManager::~ODBCManager()
	{
		DeallocateHandles();
	}

	bool ODBCManager::SetDatabase( std::string newVal )
	{
		database	= newVal;
		lastState	= true;
		return lastState;
	}
	bool ODBCManager::SetUsername( std::string newVal )
	{
		username	= newVal;
		lastState	= true;
		return lastState;
	}
	bool ODBCManager::SetPassword( std::string newVal )
	{
		password	= newVal;
		lastState	= true;
		return lastState;
	}
	bool ODBCManager::AllocateHandles( void )
	{
		if( allocationState == ALLOC_NONE )
		{
			rc = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv );
			if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
			{
				if( rc == SQL_SUCCESS_WITH_INFO )
					DisplayError( SQL_HANDLE_ENV, henv );
				allocationState = ALLOC_ENV;
				rc = SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 ); 
				if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
				{
					if( rc == SQL_SUCCESS_WITH_INFO )
						DisplayError( SQL_HANDLE_ENV, henv );
				}
				else
				{
					DisplayError( SQL_HANDLE_ENV, henv );
					DeallocateHandles();
				}
			}
			else
			{
				Console.Warning( "ODBC: ENV Handle alloc failure" );
				DisplayError( SQL_HANDLE_ENV, henv );
			}
		}
		if( allocationState == ALLOC_ENV )
		{
			rc = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );
			if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
			{
				allocationState = ALLOC_DBC;
				if( rc == SQL_SUCCESS_WITH_INFO )
					DisplayError( SQL_HANDLE_DBC, hdbc );
			}
			else
			{
				DisplayError( SQL_HANDLE_ENV, henv );
				DeallocateHandles();
				Console.Warning( "ODBC: DBC Handle alloc failure" );
			}
		}
		lastState = ( allocationState >= ALLOC_DBC );
		return lastState;
	}
	void ODBCManager::DeallocateHandles( void )
	{
		Disconnect();
		if( allocationState >= ALLOC_DBC )
			SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
		if( allocationState >= ALLOC_ENV )
			SQLFreeHandle( SQL_HANDLE_ENV, henv );
		allocationState = ALLOC_NONE;
	}
	bool ODBCManager::Connect( void )
	{
		lastState = true;
		if( AllocateHandles() )
		{
			if( allocationState >= ALLOC_DBC )
			{
				rc = SQLConnect( hdbc, (SQLCHAR *)database.c_str(), SQL_NTS, (SQLCHAR *)username.c_str(), SQL_NTS, (SQLCHAR *)password.c_str(), SQL_NTS );
				if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
				{
					allocationState = ALLOC_CONNECT;
					if( rc == SQL_SUCCESS_WITH_INFO )
						DisplayError( SQL_HANDLE_DBC, hdbc );
					rc			= SQLSetConnectAttr( hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS );
					if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
					{
						if( rc == SQL_SUCCESS_WITH_INFO )
							DisplayError( SQL_HANDLE_DBC, hdbc );
					}
					else
						DisplayError( SQL_HANDLE_DBC, hdbc );
				}
				else
				{
					if( rc == SQL_SUCCESS_WITH_INFO )
						DisplayError( SQL_HANDLE_DBC, hdbc );
					lastState = false;
					Console.Warning( "ODBC: Connection failure" );
					DeallocateHandles();
				}
			}
			else
				lastState = false;
		}
		else
			lastState = false;
		return lastState;
	}
	bool ODBCManager::Disconnect( void )
	{
		lastState = false;
		if( allocationState == ALLOC_CONNECT )
		{
			rc = SQLDisconnect( hdbc );
			if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
			{
				lastState = true;
				if( rc == SQL_SUCCESS_WITH_INFO )
					DisplayError( SQL_HANDLE_DBC, hdbc );
			}
			else
				DisplayError( SQL_HANDLE_DBC, hdbc );
			allocationState = ALLOC_DBC;
		}
		return lastState;
	}
	void ODBCManager::DisplayError( int handleType, SQLHANDLE toDiag )
	{
		SQLRETURN		rc2;
		SQLCHAR			SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER		NativeError;
		SQLSMALLINT		i = 1, MsgLen;
		while( ( rc2 = SQLGetDiagRec( handleType, toDiag, i, SqlState, &NativeError, Msg, sizeof( Msg ), &MsgLen ) ) != SQL_NO_DATA )
		{
			Console.Warning( "ODBC: SQLState   : %s", SqlState );
			Console.Warning( "ODBC: NativeError: %i", NativeError );
			Console.Warning( "ODBC: Msg        : %s", Msg );
			i++;
		}
	}

	bool ODBCManager::ExecuteQuery( std::string sql, int *index )
	{
		lastState = false;
		if( allocationState >= ALLOC_CONNECT )
		{
			if( allocationState == ALLOC_STMT && !inTransaction )
			{
				QueryRelease();
				Console.Warning( "ODBC: Forced release of query" );
			}
			HSTMT hstmt;
			rc = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &hstmt );			       //Alloc Stmt Handle 
			if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
			{
				allocationState = ALLOC_STMT;
				statementList.push_back( hstmt );
				if( index != NULL )
					(*index) = statementList.size() - 1;

				rc = SQLPrepare( hstmt, (SQLCHAR *)sql.c_str(), SQL_NTS );
				if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
				{
					rc = SQLExecute( hstmt );
					if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA )
					{
						lastState = true;
						if( rc == SQL_SUCCESS_WITH_INFO )
							DisplayError( SQL_HANDLE_STMT, hstmt );
					}
					else
					{
						Console.Warning( "ODBC: STMT Execute failure" );
						DisplayError( SQL_HANDLE_STMT, hstmt );
					}
				}
				else
				{
					Console.Warning( "ODBC: STMT Prepare failure" );
					DisplayError( SQL_HANDLE_STMT, hstmt );
				}
			}
			else
			{
				Console.Warning( "ODBC: STMT Handle alloc failure" );
				DisplayError( SQL_HANDLE_DBC, hdbc );
			}
		}
		return lastState;
	}
	bool ODBCManager::QueryRelease( void )
	{
		lastState = false;
		if( allocationState >= ALLOC_STMT )
		{
			if( !inTransaction )
				FinaliseTransaction( true );
			for( int i = 0; i < statementList.size(); ++i )
			{
				HSTMT hstmt = statementList[i];
				rc = SQLFreeHandle( SQL_HANDLE_STMT, hstmt );
				if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
				{
					lastState		= true;
					allocationState = ALLOC_CONNECT;
					if( rc == SQL_SUCCESS_WITH_INFO )
						DisplayError( SQL_HANDLE_DBC, hdbc );
				}
				else
					DisplayError( SQL_HANDLE_DBC, hdbc );
			}
			statementList.clear();
			statementList.resize( 0 );
		}
		return lastState;
	}
	bool ODBCManager::FetchRow( int index )
	{
		lastState = false;
		if( allocationState >= ALLOC_STMT )
		{
			HSTMT hstmt = statementList[index];
			rc = SQLFetch( hstmt );
			if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
			{
				if( rc == SQL_SUCCESS_WITH_INFO )
					DisplayError( SQL_HANDLE_STMT, hstmt );
				lastState = true;
			}
			else if( rc != SQL_NO_DATA )
				DisplayError( SQL_HANDLE_STMT, hstmt );
		}
		return lastState;
	}
	bool ODBCManager::GetColumn( int colNumber, UString& value, int index )
	{
		lastState = false;
		if( allocationState >= ALLOC_STMT )
		{
			if( index < statementList.size() )
			{
				char szData[256];
				SQLINTEGER cbData;
				HSTMT hstmt = statementList[index];
				rc = SQLGetData( hstmt, colNumber + 1, SQL_C_CHAR, szData, sizeof( szData ), &cbData );
				if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
				{
					lastState = true;
					if( cbData != -1 )	// NULL!
						value	= szData;
					else
						value	= "NULL";
					if( rc == SQL_SUCCESS_WITH_INFO )
						DisplayError( SQL_HANDLE_STMT, hstmt );
				}
				else if( rc == SQL_ERROR )
					DisplayError( SQL_HANDLE_STMT, hstmt );
			}
			else
				Console.Warning( "ODBC: Invalid statement number %i (max %i)", index, statementList.size() );
		}
		return lastState;
	}
	bool ODBCManager::SaveSettings( std::ofstream& target )
	{
		target << std::endl << "[ODBC]" << std::endl << "{" << std::endl;
		target << "ODBCDSN=" << database << std::endl;
		target << "ODBCUSER=" << username << std::endl;
		target << "ODBCPASS=" << password << std::endl;
		target << "}" << std::endl;
		return true;
	}
	bool ODBCManager::BeginTransaction( void )
	{
		inTransaction	= true;
		lastState		= true;
		return lastState;
	}
	bool ODBCManager::FinaliseTransaction( bool commit )
	{
		lastState = false;
		if( commit )
			rc = SQLEndTran( SQL_HANDLE_DBC, hdbc, SQL_COMMIT );
		else
			rc = SQLEndTran( SQL_HANDLE_DBC, hdbc, SQL_ROLLBACK );
		if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
		{
			lastState = true;
			if( rc == SQL_SUCCESS_WITH_INFO )
				DisplayError( SQL_HANDLE_DBC, hdbc );
		}
		else
			DisplayError( SQL_HANDLE_DBC, hdbc );
		if( inTransaction )
			QueryRelease();
		inTransaction = false;
		return lastState;
	}
	bool ODBCManager::LastSucceeded( void )
	{
		return lastState;
	}
}