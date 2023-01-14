#include "assertion_logger.h"

#include <base/logger.h>
#include <base/system.h>
#include <engine/shared/ringbuffer.h>
#include <engine/storage.h>

#include <mutex>

class CAssertionLogger : public ILogger
{
	void Dump();

	struct SDebugMessageItem
	{
		char m_aMessage[1024];
	};

	std::mutex m_DbgMessageMutex;
	CStaticRingBuffer<SDebugMessageItem, sizeof(SDebugMessageItem) * 64, CRingBufferBase::FLAG_RECYCLE> m_DbgMessages;

	char m_aAssertLogPath[IO_MAX_PATH_LENGTH];
	char m_aGameName[256];

public:
	CAssertionLogger(const char *pAssertLogPath, const char *pGameName);
	void Log(const CLogMessage *pMessage) override;
	void GlobalFinish() override;
};

void CAssertionLogger::Log(const CLogMessage *pMessage)
{
	std::unique_lock<std::mutex> Lock(m_DbgMessageMutex);
	SDebugMessageItem *pMsgItem = (SDebugMessageItem *)m_DbgMessages.Allocate(sizeof(SDebugMessageItem));
	str_copy(pMsgItem->m_aMessage, pMessage->m_aLine);
}

void CAssertionLogger::GlobalFinish()
{
	if(dbg_assert_has_failed())
	{
		Dump();
	}
}

void CAssertionLogger::Dump()
{
	char aAssertLogFile[IO_MAX_PATH_LENGTH];
	char aDate[64];
	str_timestamp(aDate, sizeof(aDate));
	str_format(aAssertLogFile, std::size(aAssertLogFile), "%s%s_assert_log_%d_%s.txt", m_aAssertLogPath, m_aGameName, pid(), aDate);
	std::unique_lock<std::mutex> Lock(m_DbgMessageMutex);
	IOHANDLE FileHandle = io_open(aAssertLogFile, IOFLAG_WRITE);
	if(FileHandle)
	{
		auto *pIt = m_DbgMessages.First();
		while(pIt)
		{
			io_write(FileHandle, pIt->m_aMessage, str_length(pIt->m_aMessage));
			io_write(FileHandle, "\n", 1);

			pIt = m_DbgMessages.Next(pIt);
		}

		io_sync(FileHandle);
		io_close(FileHandle);
	}
}

CAssertionLogger::CAssertionLogger(const char *pAssertLogPath, const char *pGameName)
{
	str_copy(m_aAssertLogPath, pAssertLogPath);
	str_copy(m_aGameName, pGameName);
}

std::unique_ptr<ILogger> CreateAssertionLogger(IStorage *pStorage, const char *pGameName)
{
	char aAssertLogPath[IO_MAX_PATH_LENGTH];
	pStorage->GetCompletePath(IStorage::TYPE_SAVE, "dumps/", aAssertLogPath, sizeof(aAssertLogPath));
	return std::make_unique<CAssertionLogger>(aAssertLogPath, pGameName);
}
