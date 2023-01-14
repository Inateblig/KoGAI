/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>

#include <game/generated/protocol.h>

#include <game/client/gameclient.h>

#include <game/client/components/motd.h>
#include <game/client/components/scoreboard.h>

#include "broadcast.h"

void CBroadcast::OnReset()
{
	m_BroadcastTick = 0;
}

void CBroadcast::OnRender()
{
	if(m_pClient->m_Scoreboard.Active() || m_pClient->m_Motd.IsActive() || !g_Config.m_ClShowBroadcasts)
		return;

	Graphics()->MapScreen(0, 0, 300 * Graphics()->ScreenAspect(), 300);

	if(Client()->GameTick(g_Config.m_ClDummy) < m_BroadcastTick)
	{
		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, m_BroadcastRenderOffset, 40.0f, 12.0f, TEXTFLAG_RENDER | TEXTFLAG_STOP_AT_END);
		Cursor.m_LineWidth = 300 * Graphics()->ScreenAspect() - m_BroadcastRenderOffset;
		TextRender()->TextEx(&Cursor, m_aBroadcastText, -1);
	}
}

void CBroadcast::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_BROADCAST)
	{
		CNetMsg_Sv_Broadcast *pMsg = (CNetMsg_Sv_Broadcast *)pRawMsg;
		str_copy(m_aBroadcastText, pMsg->m_pMessage);
		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, 0, 0, 12.0f, TEXTFLAG_STOP_AT_END);
		Cursor.m_LineWidth = 300 * Graphics()->ScreenAspect();
		TextRender()->TextEx(&Cursor, m_aBroadcastText, -1);
		m_BroadcastRenderOffset = 150 * Graphics()->ScreenAspect() - Cursor.m_X / 2;
		m_BroadcastTick = Client()->GameTick(g_Config.m_ClDummy) + Client()->GameTickSpeed() * 10;
		if(g_Config.m_ClPrintBroadcasts)
		{
			char aBuf[1024];
			int i, ii;
			for(i = 0, ii = 0; i < str_length(m_aBroadcastText); i++)
			{
				if(m_aBroadcastText[i] == '\n')
				{
					aBuf[ii] = '\0';
					ii = 0;
					if(aBuf[0])
						m_pClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "broadcast", aBuf, color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClMessageHighlightColor)));
				}
				else
				{
					aBuf[ii] = m_aBroadcastText[i];
					ii++;
				}
			}
			aBuf[ii] = '\0';
			if(aBuf[0])
				m_pClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "broadcast", aBuf, color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClMessageHighlightColor)));
		}
	}
}
