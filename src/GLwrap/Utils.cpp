#include "GLwrap/Utils.h"

std::string GLwrap::get_file_contents(std::string filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

glm::vec3 GLwrap::randomVec3(std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(0.0f, 127.0f);
    return glm::vec3(dist(rng), dist(rng), dist(rng));
}