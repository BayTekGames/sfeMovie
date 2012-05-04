
/*
 *  Movie_audio.hpp
 *  sfeMovie project
 *
 *  Copyright (C) 2010-2012 Lucas Soltic
 *  soltic.lucas@gmail.com
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


#ifndef MOVIE_AUDIO_HPP
#define MOVIE_AUDIO_HPP

extern "C" 
{ 
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h>
}
#include <queue>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

namespace sfe {
	class Movie;
	class Movie_audio : private sf::SoundStream {
	public:
		Movie_audio(Movie& parent);
		~Movie_audio(void);
		
		// -------------------------- Audio methods ----------------------------
		bool initialize(void);
		void stop(void);
		void close(void);
		
		using sf::SoundStream::play;
		using sf::SoundStream::pause;
		using sf::SoundStream::setVolume;
		using sf::SoundStream::getVolume;
		using sf::SoundStream::getSampleRate;
		using sf::SoundStream::getChannelCount;
		using sf::SoundStream::setPlayingOffset;
		using sf::SoundStream::getPlayingOffset;
		
		void preSeek(sf::Time position);
		void postSeek(sf::Time position);
		void loadSample();
		
		int getStreamID() const;
		AVCodecContext *getCodecContext(void) const;
		sf::Int64 getLatestPacketTimestamp(void) const;
		void updateTimestamp(sf::Int64 timestamp);
		bool isStarving(void);
		
		bool readChunk(void);
		bool hasPendingDecodableData(void);
		unsigned currentlyPendingDataLength(void);
		void decodeFrontFrame(Chunk& sfBuffer);
		void pushFrame(AVPacket *pkt);
		void popFrame(void);
		AVPacket *frontFrame(void);
		AVPacket *takeFrontFrame(void);
		void flushPendingFrames(void);
		
		bool onGetData(Chunk& Data);
		void onSeek(sf::Time timeOffset);
		
	private:
		// ------------------------- Audio attributes --------------------------
		Movie& m_parent;
		
		// FFmpeg stuff
		AVCodecContext *m_codecCtx; 
		AVCodec *m_codec;
		std::queue <AVPacket *> m_packetList;

		sf::Mutex m_packetListMutex;
		int m_streamID;
		sf::Int64 m_latestPacketTimestamp; // The latest non-null timestamp extracted from the latest decoded audio packet
		sf::Int16 *m_buffer; // Buffer used to store the current audio data chunk
		unsigned m_pendingDataLength;
		
		unsigned m_channelsCount;
		unsigned m_sampleRate;
		bool m_isStarving;
		sf::Time m_prevPlayingOffset;
	}; // class Movie_audio
} // namespace sfe

#endif
