#include "SubtitleFactory.h"

#include <fstream>
#include <iostream>

string getFileExtension(const string& filename)
{
  size_t dotPosition = filename.rfind('.');
  if (dotPosition == string::npos)
  {
    return "";
  }
  return filename.substr(dotPosition);
}

string fileDetection(ifstream& in)
{
  string line;
  getline(in, line);
  line.erase(0, line.find_first_not_of(" \t\r\n"));
  line.erase(line.find_last_not_of(" \t\r\n") + 1);
  if (line == "0" || line == "1")
  {
    return ".srt";
  }
  else if (line == "<SAMI>")
  {
    return ".smi";
  }
  else if (line == "[Script Info]")
  {
    return ".ass";
  }
  else
  {
    return ".unknown";
  }
}

int main(int argc, char* argv[])
{
  ifstream in(argv[1]);
  ofstream out(argv[2]);
  if (!in.is_open())
  {
    cout << "Failed to open file " << argv[1] << "\n";
    return 1;
  }
  string format = fileDetection(in);
  SubtitleFactory s;
  auto sub = s.create(format);
  sub->fileParse(in);
  string extension = getFileExtension(argv[2]);
  if (extension == ".srt")
  {
    unique_ptr< WriteBehavior > writer = make_unique< toSRT >();
    writer->write(out, sub->getContents());
  }
  else if (extension == ".smi")
  {
    unique_ptr< WriteBehavior > writer = make_unique< toSAMI >();
    writer->write(out, sub->getContents());
  }
  else if (extension == ".ass")
  {
    unique_ptr< WriteBehavior > writer = make_unique< toSSA >();
    writer->write(out, sub->getContents());
  }
  else if (extension == ".ttml")
  {
    unique_ptr< WriteBehavior > writer = make_unique< toTTML >();
    writer->write(out, sub->getContents());
  }
}
