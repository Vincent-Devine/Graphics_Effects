#pragma once

class Texture
{
	// Attributs
private:
	unsigned int id;

	// Methods
public:
	Texture();
	Texture(const char* p_path, bool gammaCorrection);

	const unsigned int GetTexture() { return id; };

	void Draw() const;

private:
	const unsigned int Init(const char* p_path, bool gammaCorrection);
};