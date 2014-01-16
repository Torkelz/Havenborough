#pragma once

/**
 * We should probably place some license information here.
 */

#include <exception>
#include <string>

/**
 * Base class for all our exceptions, all our exceptions
 * should inherit from this class.
 */
class ClientException : public std::exception
{
protected:
	/**
	 * Message describing the error
	 */
	const std::string m_What;
	/**
	 * The line number where the exception was thrown
	 */
	int m_Line;
	/**
	 * The file the exception was thrown from
	 */
	const std::string m_File;

public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	ClientException(const std::string& p_What, int p_Line, const std::string& p_File)
		: m_What(p_File + "(" + std::to_string(p_Line) + "): " + p_What),
		  m_Line(p_Line),
		  m_File(p_File)
	{
	}

	virtual const char* what() const throw() override
	{
		return m_What.c_str();
	}
};

/**
 * An exception to be thrown when an error is encountered during window management.
 */
class WindowException : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	WindowException(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};

/**
 * An exception to be thrown when an error is returned from win32 functions.
 */
class Win32Exception : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	Win32Exception(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};

class SceneManagerException : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	SceneManagerException(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};

/**
 * An exception to be thrown when an invalid argument has been passed to a function.
 */
class InvalidArgument : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	InvalidArgument(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};


class ResourceManagerException : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	
	ResourceManagerException(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};


/**
* An exception to be thrown when an error occurs with edge collision and handling
*/
class EdgeCollisionException : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	
	EdgeCollisionException(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};

/**
* An exception to be thrown when an error occurs in a scene or within the scene manager.
*/
class SceneException : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	
	SceneException(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};

/**
* An exception to be thrown when an error occurs with event messaging.
*/
class EventException : public ClientException
{
public:
	/**
	 * constructor.
	 *
	 * @param p_What A message describing the error
	 * @param p_Line The line of the exception (use __LINE__)
	 * @param p_File The file of the exception (use __FILE__)
	 */
	
	EventException(const std::string& p_What, int p_Line, const std::string& p_File)
		: ClientException(p_What, p_Line, p_File)
	{
	}
};