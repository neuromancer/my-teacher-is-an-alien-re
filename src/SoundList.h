#ifndef SOUNDLIST_H
#define SOUNDLIST_H

class SoundList {
public:
  SoundList(int count);
  ~SoundList();
  void StopAll();
  void *Register(char *filename);

private:
  void **m_sounds;
  int m_count;
  void **m_field8;
  short m_fieldc;
  short m_pad;
};

#endif // SOUNDLIST_H
