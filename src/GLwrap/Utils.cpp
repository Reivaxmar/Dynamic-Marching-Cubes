#include "GLwrap/Utils.h"


namespace GLwrap
{
	
	std::string get_file_contents(std::string filename)
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
	
	glm::vec3 randomVec3(std::mt19937& rng) {
		std::uniform_real_distribution<float> dist(0.0f, 127.0f);
		return glm::vec3(dist(rng), dist(rng), dist(rng));
	}


    void saveToFile(
		std::string filename,
		const std::vector<glm::vec4>& points,
		const std::vector<glm::vec4>& normals
	) {
		// Write OBJ file
		const size_t vertexCount = points.size();
		std::ofstream objFile("output.obj");
		if (!objFile) {
			std::cerr << "Failed to open output.obj for writing\n";
		}

		// Write positions
		std::cout << "Writing vertices\n";
		for (const auto& v : points) {
			objFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
		}

		// Write normals
		std::cout << "Writing normals\n";
		for (const auto& n : normals) {
			objFile << "vn " << n.x << " " << n.y << " " << n.z << "\n";
		}
		
		// Write faces
		std::cout << "Writing faces\n";
		for (size_t i = 0; i < vertexCount; i += 3) {
			objFile << "f " << i + 1 << " " << i + 2 << " " << i + 3 << "\n";
		}

		objFile.close();
		std::cout << "Saved mesh to output.obj\n";
	}
} // namespace GLwrap

