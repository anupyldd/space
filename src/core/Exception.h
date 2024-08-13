
#pragma once

#include <exception>
#include <format>
#include <source_location>

#include "Logging.h"

// API ---------------------------------

namespace exc
{
	class CoreException;
}

// -------------------------------------

namespace exc
{
	class IException : public std::exception
	{
	public:
		IException() = default;
		IException(const String& msg, std::source_location loc = std::source_location::current())
			: m_msg(msg), m_loc(loc) {}

		String What() const noexcept
		{
			return std::format("[{}] {}.\n          Function: {}\n          File: {} (line {})",
				m_Name(), m_msg,
				m_loc.function_name(), m_loc.file_name(), m_loc.line());
		}
	protected:
		virtual String m_Name() const = 0;
		std::source_location m_loc;
		String m_msg;
	};

	class CoreException : public IException
	{
	public:
		CoreException() = default;
		CoreException(const String& msg, std::source_location loc = std::source_location::current()) : IException(msg, loc) {}

	protected:
		virtual String m_Name() const override { return "Core Exception"; }
	};

	class RenderException : public IException
	{
	public:
		RenderException() = default;
		RenderException(const String& msg, std::source_location loc = std::source_location::current()) : IException(msg, loc) {}

	protected:
		virtual String m_Name() const override { return "Render Exception"; }
	};

	class EngineException : public IException
	{
	public:
		EngineException() = default;
		EngineException(const String& msg, std::source_location loc = std::source_location::current()) : IException(msg, loc) {}

	protected:
		virtual String m_Name() const override { return "Engine Exception"; }
	};

	class GameException : public IException
	{
	public:
		GameException() = default;
		GameException(const String& msg, std::source_location loc = std::source_location::current()) : IException(msg, loc) {}

	protected:
		virtual String m_Name() const override { return "Game Exception"; }
	};
}
