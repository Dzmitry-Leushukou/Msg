#include "Image.h"


Image::Image(std::string sender, std::string format,std::string data, std::string time)
{
	this->sender = sender;
	this->format = format;
	this->data = data;
    timestamp = time;
}

void Image::open()
{
	if (!std::filesystem::exists(filepath))
		throw std::invalid_argument("Can`t open image. Try to save again");
    std::string cmd = "start " + filepath;
    system(cmd.c_str());
}

void Image::save(std::string filepath)
{
    std::ofstream fout(filepath+"."+format);
    if (!fout)
    {
        throw std::invalid_argument("Can`t save image in this location");
    }
    fout << data;
    fout.close();
    this->filepath = filepath + "." + format;
}

std::string Image::to_string()
{
    return sender + ": [IMG]\nSave to open\n";
}