
#ifndef _MCU_FILEMEMBERS_H
#define _MCU_FILEMEMBERS_H

#include "config.h"

#include <ptlib/sound.h>
#include <opalwavfile.h>
#include "conference.h"

#include <deque>


class MCUH323Connection;

class ConferenceSoundCardMember : public ConferenceMember
{
  PCLASSINFO(ConferenceSoundCardMember, ConferenceMember);
  public:
    ConferenceSoundCardMember(Conference * _conference);
    ~ConferenceSoundCardMember();

    void Unlisten();

    virtual ConferenceConnection * CreateConnection()
    { return new ConferenceConnection(this); }

    virtual PString GetName() const
    { return "sound card listener"; }

    virtual BOOL IsVisible() const
    { return FALSE; }

    void OnReceivedUserInputIndication(const PString & str)
    { cout << "Received user input indication " << str << endl; }

    PDECLARE_NOTIFIER(PThread, ConferenceSoundCardMember, Thread);

  protected:
    BOOL running;
    PSoundChannel soundDevice;
    PThread * thread;
};

class ConferenceFileMember : public ConferenceMember
{
  PCLASSINFO(ConferenceFileMember, ConferenceMember);
  public:
    ConferenceFileMember(Conference * conference, const PFilePath & _fn, PFile::OpenMode mode);

    typedef std::deque<PFilePath> FilenameList;
    ConferenceFileMember(Conference * conference, const FilenameList & _fns, PFile::OpenMode mode);

    ConferenceFileMember(Conference * conference, const OpalMediaFormat & _fmt, PFile::OpenMode mode);
    ConferenceFileMember(Conference * conference, const OpalMediaFormat & _fmt, PFile::OpenMode mode, unsigned _videoMixerNumber);

    ConferenceFileMember(Conference * conference, const PString & _fmt, PFile::OpenMode mode);

    ~ConferenceFileMember();

    void Unlisten();

    virtual ConferenceConnection * CreateConnection()
    { return new ConferenceConnection(this); }

    virtual PString GetName() const
    {
      if(vformat.GetLength()>0)
      {
        return "cache";
      }
      return PString(mode == PFile::ReadOnly ? "file player" : "file recorder") & currentFilename;
    }

    virtual MemberTypes GetType()
    {
      if(GetName() == "cache")
        return MEMBER_TYPE_CACHE;
      return MEMBER_TYPE_PIPE;
    }

    virtual BOOL IsVisible() const
    { return FALSE; }

    void OnReceivedUserInputIndication(const PString & str)
    { cout << "Received user input indication " << str << endl; }

    PDECLARE_NOTIFIER(PThread, ConferenceFileMember, ReadThread);
    PDECLARE_NOTIFIER(PThread, ConferenceFileMember, WriteThread);
    PDECLARE_NOTIFIER(PThread, ConferenceFileMember, WriteThreadV);
    PDECLARE_NOTIFIER(PThread, ConferenceFileMember, EncoderCacheThread);

    virtual PString GetFormat(){ return format; }
    virtual PString GetVFormat(){ return vformat; }

    H323Codec * codec;
    MCUH323Connection * con;
    int status;

  protected:
    void Construct();
    BOOL QueueNext();

    FilenameList filenames;
    PFilePath currentFilename;
    PString format;
    OpalMediaFormat vformat;
    PFile::OpenMode mode;

    OpalWAVFile file;
    BOOL running;
    PThread * thread;
    PThread * vthread;

    PString roomName;
#ifndef _WIN32
    PString audioPipeName, videoPipeName;
#endif
};

class ConferenceFilePlayer : public ConferenceFileMember
{
  PCLASSINFO(ConferenceFilePlayer, ConferenceFileMember);
  public:
    ConferenceFilePlayer();

    void Unlisten();

    PDECLARE_NOTIFIER(PThread, ConferenceFilePlayer, Thread);
};

#endif // _MCU_FILEMEMBERS_H
