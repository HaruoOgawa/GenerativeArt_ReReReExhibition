#include "SoundPlayer.h"
#include <Windows.h>
#include <mmsystem.h>
#include <array>
#pragma comment(lib, "Winmm.lib")
#ifdef _DEBUG
#include <sstream>
#endif // _DEBUG

namespace sound 
{
	SoundPlayer::SoundPlayer():
		m_IsMute(false)
	{
		Initialize();
	}

	SoundPlayer::~SoundPlayer() {
		Release();
	}

	bool SoundPlayer::Initialize() {
		char ExePath[256];
		GetModuleFileName(NULL, ExePath, 256);
		std::string ExeDir = GetExeDir(ExePath);

		std::string AudioPath ="\"" + ExeDir + "\\" + "bin\\electronica_electronic_edm_.mp3" + "\"";
		std::string cmd = "open " + AudioPath + " type mpegvideo alias mp3";
		
		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA(
				cmd.c_str(),
				nullptr,
				0,
				nullptr),
			errorString.data(),
			MAXERRORLENGTH);
		//std::printf("%s\n", errorString.data());

		return true;
	}

	bool SoundPlayer::Play() 
	{
		if (m_IsMute) return Pause();

		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA("play mp3", nullptr, 0, nullptr),
			errorString.data(),
			MAXERRORLENGTH);
		//std::printf("%s\n", errorString.data());

		return true;
	}

	bool SoundPlayer::Pause() {
		mciSendStringA("pause mp3", NULL, 0, NULL);

		return true;
	}

	void SoundPlayer::Skip(float SkipOffset)
	{
#ifdef _DEBUG
		float Offset = (SkipOffset * 1000.0f); // ミリ秒に直す

		std::ostringstream ss;
		ss << Offset;
		std::string SkipOffset_str(ss.str());
		std::string cmd = "seek mp3 to " + SkipOffset_str;

		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA(cmd.c_str(), NULL, 0, NULL),
			errorString.data(),
			MAXERRORLENGTH);

		// スキップした後は、もう一度Playしないと音が鳴らない
		Play();
#endif // _DEBUG

	}

	void SoundPlayer::Mute(bool IsMute)
	{
		m_IsMute = IsMute;
	}

	void SoundPlayer::Release() {
		mciSendStringA("close mp3", NULL, 0, NULL);
	}

	std::string SoundPlayer::GetExeDir(char path[]) {
		std::string path_str(path);
		int pathBlockOrder=path_str.rfind("\\");
		std::string ExeDir = path_str.erase(pathBlockOrder);
		return ExeDir;
	}
}
