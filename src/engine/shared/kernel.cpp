/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/system.h>
#include <engine/kernel.h>

class CKernel : public IKernel
{
	enum
	{
		MAX_INTERFACES = 32,
	};

	class CInterfaceInfo
	{
	public:
		CInterfaceInfo()
		{
			m_aName[0] = 0;
			m_pInterface = 0x0;
			m_AutoDestroy = false;
		}

		char m_aName[64];
		IInterface *m_pInterface;
		bool m_AutoDestroy;
	};

	CInterfaceInfo m_aInterfaces[MAX_INTERFACES];
	int m_NumInterfaces;

	CInterfaceInfo *FindInterfaceInfo(const char *pName)
	{
		for(int i = 0; i < m_NumInterfaces; i++)
		{
			if(str_comp(pName, m_aInterfaces[i].m_aName) == 0)
				return &m_aInterfaces[i];
		}
		return 0x0;
	}

public:
	CKernel()
	{
		m_NumInterfaces = 0;
	}

	void Shutdown() override
	{
		for(int i = m_NumInterfaces - 1; i >= 0; i--)
		{
			if(m_aInterfaces[i].m_AutoDestroy)
				m_aInterfaces[i].m_pInterface->Shutdown();
		}
	}

	virtual ~CKernel()
	{
		// delete interfaces in reverse order just the way it would happen to objects on the stack
		for(int i = m_NumInterfaces - 1; i >= 0; i--)
		{
			if(m_aInterfaces[i].m_AutoDestroy)
			{
				delete m_aInterfaces[i].m_pInterface;
				m_aInterfaces[i].m_pInterface = 0;
			}
		}
	}

	bool RegisterInterfaceImpl(const char *pName, IInterface *pInterface, bool Destroy) override
	{
		// TODO: More error checks here
		if(!pInterface)
		{
			dbg_msg("kernel", "ERROR: couldn't register interface %s. null pointer given", pName);
			return false;
		}

		if(m_NumInterfaces == MAX_INTERFACES)
		{
			dbg_msg("kernel", "ERROR: couldn't register interface '%s'. maximum of interfaces reached", pName);
			return false;
		}

		if(FindInterfaceInfo(pName) != 0)
		{
			dbg_msg("kernel", "ERROR: couldn't register interface '%s'. interface already exists", pName);
			return false;
		}

		pInterface->m_pKernel = this;
		m_aInterfaces[m_NumInterfaces].m_pInterface = pInterface;
		str_copy(m_aInterfaces[m_NumInterfaces].m_aName, pName);
		m_aInterfaces[m_NumInterfaces].m_AutoDestroy = Destroy;
		m_NumInterfaces++;

		return true;
	}

	bool ReregisterInterfaceImpl(const char *pName, IInterface *pInterface) override
	{
		if(FindInterfaceInfo(pName) == 0)
		{
			dbg_msg("kernel", "ERROR: couldn't reregister interface '%s'. interface doesn't exist", pName);
			return false;
		}

		pInterface->m_pKernel = this;

		return true;
	}

	IInterface *RequestInterfaceImpl(const char *pName) override
	{
		CInterfaceInfo *pInfo = FindInterfaceInfo(pName);
		if(!pInfo)
		{
			dbg_msg("kernel", "failed to find interface with the name '%s'", pName);
			return 0;
		}
		return pInfo->m_pInterface;
	}
};

IKernel *IKernel::Create() { return new CKernel; }
