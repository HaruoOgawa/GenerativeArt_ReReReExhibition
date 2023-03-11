#include "SoundPlayer.h"
#include <Windows.h>
#include <mmsystem.h>
#include <array>
#pragma comment(lib, "Winmm.lib")
#include <sstream>
#include "GraphicsEngine/Message/Console.h"

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

		std::string AudioPath = "\"" + ExeDir + "\\" + "silentspace_nullptr_monet_keyboard.wav" + "\"";
		std::string cmd = "open " + AudioPath + " alias wav";
		
		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA(
				cmd.c_str(),
				nullptr,
				0,
				nullptr),
			errorString.data(),
			MAXERRORLENGTH);
		//Console::Log(">>>>>>>>>>>>>>>>>>[Audio Error Log] %s / AudioPath: %s\n", errorString.data(), AudioPath.c_str());

		return true;
	}

	bool SoundPlayer::Play() 
	{
		if (m_IsMute) return Pause();

		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA("play wav", nullptr, 0, nullptr),
			errorString.data(),
			MAXERRORLENGTH);
		//Console::Log(">>>>>>>>>>>>>>>>>>[Audio Error Log] %s\n", errorString.data());
		return true;
	}

	bool SoundPlayer::Pause() {
		mciSendStringA("pause wav", NULL, 0, NULL);

		return true;
	}

	void SoundPlayer::Skip(float SkipOffset)
	{
		float Offset = (SkipOffset * 1000.0f); // ミリ秒に直す

		std::ostringstream ss;
		ss << Offset;
		std::string SkipOffset_str(ss.str());
		std::string cmd = "seek wav to " + SkipOffset_str;

		std::array<char, MAXERRORLENGTH> errorString;
		mciGetErrorStringA(
			mciSendStringA(cmd.c_str(), NULL, 0, NULL),
			errorString.data(),
			MAXERRORLENGTH);

		// スキップした後は、もう一度Playしないと音が鳴らない
		Play();
	}

	void SoundPlayer::Mute(bool IsMute)
	{
		m_IsMute = IsMute;
	}

	void SoundPlayer::Release() {
		mciSendStringA("close wav", NULL, 0, NULL);
	}

	std::string SoundPlayer::GetExeDir(char path[]) {
		std::string path_str(path);
		int pathBlockOrder=path_str.rfind("\\");
		std::string ExeDir = path_str.erase(pathBlockOrder);
		return ExeDir;
	}
}
