/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_SOUNDS_H
#define GAME_CLIENT_COMPONENTS_SOUNDS_H

#include <base/vmath.h>
#include <engine/shared/jobs.h>
#include <engine/sound.h>
#include <game/client/component.h>

class CSoundLoading : public IJob
{
	CGameClient *m_pGameClient;
	bool m_Render;

public:
	CSoundLoading(CGameClient *pGameClient, bool Render);
	void Run() override;
};

class CSounds : public CComponent
{
	enum
	{
		QUEUE_SIZE = 32,
	};
	struct QueueEntry
	{
		int m_Channel;
		int m_SetId;
	} m_aQueue[QUEUE_SIZE];
	int m_QueuePos;
	int64_t m_QueueWaitTime;
	std::shared_ptr<CSoundLoading> m_pSoundJob;
	bool m_WaitForSoundJob;

	int GetSampleId(int SetId);

	float m_GuiSoundVolume;
	float m_GameSoundVolume;
	float m_MapSoundVolume;
	float m_BackgroundMusicVolume;

public:
	// sound channels
	enum
	{
		CHN_GUI = 0,
		CHN_MUSIC,
		CHN_WORLD,
		CHN_GLOBAL,
		CHN_MAPSOUND,
	};

	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnInit() override;
	virtual void OnReset() override;
	virtual void OnStateChange(int NewState, int OldState) override;
	virtual void OnRender() override;

	void ClearQueue();
	void Enqueue(int Channel, int SetId);
	void Play(int Channel, int SetId, float Vol);
	void PlayAt(int Channel, int SetId, float Vol, vec2 Pos);
	void PlayAndRecord(int Channel, int SetId, float Vol, vec2 Pos);
	void Stop(int SetId);
	bool IsPlaying(int SetId);

	ISound::CVoiceHandle PlaySample(int Channel, int SampleId, float Vol, int Flags = 0);
	ISound::CVoiceHandle PlaySampleAt(int Channel, int SampleId, float Vol, vec2 Pos, int Flags = 0);
};

#endif
