#include "Geometrie.h"



Geometrie::Geometrie():terrainFilename(0),terrainHeight(0),colorMapFilename(0)
{
}


Geometrie::~Geometrie()
{
	ShutdownHeightMap();
}

void Geometrie::CreateTriangleGrid(float width, float depth, UINT m, UINT n, MeshData& meshData)
{
	UINT vertexCount = m*n; UINT faceCount = (m - 1)*(n - 1) * 2;

	// 
	// Create the vertices.
	// 
	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;
	float dx = width / (n - 1);
	float dz = depth / (m - 1);

		float du = 1.0f / (n-1); 
		float dv = 1.0f / (m-1); 

	meshData.Vertices.resize(vertexCount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dz;

		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;
			meshData.Vertices[i*n + j].Position = XMFLOAT3(x, 0.0f, z);

			// Ignore for now, used for lighting.
			meshData.Vertices[i*n+j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.Vertices[i*n+j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f); 

			// Ignore for now, used for texturing. 
			meshData.Vertices[i*n+j].TexC.x = j*du;
			meshData.Vertices[i*n+j].TexC.y = i*dv;
		}
	}
	meshData.Indices.resize(faceCount * 3); // 3 indices per face
	
	// Iterate over each quad and compute indices.
	UINT k = 0; 
	for(UINT i = 0; i < m-1; ++i) 
	{
		for(UINT j = 0; j < n-1; ++j)
		{ 
			meshData.Indices[k] = i*n+j;
			meshData.Indices[k+1] = i*n+j+1; 
			meshData.Indices[k+2] = (i+1)*n+j; 
			meshData.Indices[k+3] = (i+1)*n+j; 
			meshData.Indices[k+4] = i*n+j+1; 
			meshData.Indices[k+5] = (i+1)*n+j+1;
			k += 6; // next quad
		}
	}
}
bool Geometrie::CreateHeightMapGrid(char* filename,MeshData & data)
{
	bool result;

	result =LoadSetupFile(filename);
	
	if (!result)
	{
		MessageBox(0, L"errore filename", L"attenzione", 0);
		return false;
	}

	result = LoadBitmapHeightMap();

	if (!result)
	{
		MessageBox(0, L"errore bitmap", L"attenzione", 0);
		return false;
	}

	SetTerrainCoordinates();

	result = CalculateNormals();
	
	if (!result)
	{
		return false;
	}
	// color map

	result = LoadColorMap();

	if (!result)
	{
		MessageBox(0, L"errore color Map", L"attenzione", 0);
		return false;
	}
	result = BuildTerrainModel(data);

	if (!result)
	{
		MessageBox(0, L"errore terrain Model", L"attenzione", 0);
		return false;
	}
	ShutdownHeightMap();

}
// ASSIMP LOADER
bool Geometrie::CreaMesh(const char* filename,bool flipUVs)
{
	Assimp::Importer importer;

	UINT flags =  aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;

	if (flipUVs)
	{
		flags |= aiProcess_FlipUVs;
	}

	const aiScene* scene = importer.ReadFile(filename, flags);
	
	if (scene == 0)
	{
		return false;
	}

	processoRicorsivo(scene->mRootNode, scene);
	return true;
}
void Geometrie::processMesh(aiMesh* mesh, const aiScene* scene)
{
	MeshData data;
	Vertex Vertici;

	//data.Vertices.resize(mesh->mNumVertices);
	//data.Vertices.clear();

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			// position
			Vertici.Position.x = mesh->mVertices[i].x;
			Vertici.Position.y = mesh->mVertices[i].y;
			Vertici.Position.z = mesh->mVertices[i].z;
			
			// normal
			if (mesh->HasNormals())
			{
				Vertici.Normal.x = mesh->mNormals[i].x;
				Vertici.Normal.y = mesh->mNormals[i].y;
				Vertici.Normal.z = mesh->mNormals[i].z;
			}
			else
			{
				Vertici.Normal = { 0.0f, 1.0f ,0.0f };
			}
			// tangent
			if (mesh->HasTangentsAndBitangents())
			{
				Vertici.TangentU.x = mesh->mTangents[i].x;
				Vertici.TangentU.y = mesh->mTangents[i].y;
				Vertici.TangentU.z = mesh->mTangents[i].z;
			}
			else
			{
				Vertici.TangentU = { 0,0,0 };
			}

			// color
			if (mesh->HasVertexColors(0))
			{
				Vertici.Color.x = mesh->mColors[0][i].r;
				Vertici.Color.y = mesh->mColors[0][i].g;
				Vertici.Color.z = mesh->mColors[0][i].b;
			}
			else
			{
				Vertici.Color = { 0,0,0 };
			}

			// textureCoord
			if (mesh->HasTextureCoords(0))
			{
				Vertici.TexC.x = mesh->mTextureCoords[0][i].x;
				Vertici.TexC.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				Vertici.TexC = { 0,0 };
			}

			data.Vertices.push_back(Vertici);
		}
		
		
	/*	int indices = 0;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			indices += face.mNumIndices;
		}
		data.Indices.resize(indices);
		data.Indices.clear();
		*/
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				data.Indices.push_back(face.mIndices[j]);
			}
		}

		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++)
		{
			aiString str;
			mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
			
			std::wstring wPath;
			std::string string = str.C_Str();

			wPath.assign(string.begin(), string.end());

			data.Textures.push_back(wPath);
		}
		meshes.push_back(data);
		
}
void Geometrie::processoRicorsivo(aiNode* node, const aiScene* scene)
{
	// processo

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	// ricorsione
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processoRicorsivo(node->mChildren[i], scene);
	}
}
// HEIGHT MAPS & Color Maps
bool Geometrie::LoadSetupFile(char* filename)
{

	std::ifstream fin;
	char input;

	int lunghezzaStringa = 256;

	terrainFilename = new char[lunghezzaStringa];

	if (!terrainFilename)
	{
		return false;
	}

	colorMapFilename = new char[lunghezzaStringa];

	if (!colorMapFilename)
	{
		return false;
	}

	// apro il file

	fin.open(filename);

	if (fin.fail())
	{
		return false;
	}

	// leggo
	do
	{
		fin.get(input);

	} while (input != ':');
	// inserisco il filename del file bmp
	fin >> terrainFilename;

	// leggo
	do
	{
		fin.get(input);

	} while (input != ':');
	// inserisco il valore height
	fin >> terrainHeight;

	// leggo
	do
	{
		fin.get(input);

	} while (input != ':');
	// inserisco il valore witdh
	fin >> terrainWidth;

	// leggo
	do
	{
		fin.get(input);

	} while (input != ':');
	
	// inserisco il valore height scale
	fin >> heightScale;
	 
	// leggo
	do
	{
		fin.get(input);

	} while (input != ':');

	fin >> colorMapFilename;

	// Close 
	fin.close();


	return true;
}
bool Geometrie::LoadBitmapHeightMap()
{
	int errore, imageSize, index;

	FILE* filePtr;
	UINT count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	UCHAR* bitmapImage;
	UCHAR height;

	heightMap = new HeightMap[terrainWidth*terrainHeight];
	
	if (!heightMap)
	{
		return false;
	}

	// apro il file bitmap in modalità binaria

	errore = fopen_s(&filePtr, terrainFilename, "rb");
	
	if (errore != 0)
	{
		return false;
	}

	// leggo il file header nel bitmap
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	
	if(count != 1)
	{
		return false;
	}

	// leggo il file info nel bitmap
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	
	if (count != 1)
	{
		return false;
	}
	
	// mi assicuro che la dimensione dell'heightmap siano le stesse del terreno per fare un mapping 1 ad 1
	
	if ((bitmapInfoHeader.biHeight != terrainHeight) || (bitmapInfoHeader.biWidth != terrainWidth))
	{
		return false;
	}

	// calcolo la dimensione dei dati dell'immagine bitmap
	// siccome non divido per 2 dimensioni (es. 257x257) devo aggiunge un byte extra per ogni linea
	imageSize = terrainHeight*((terrainWidth * 3) + 1);

	// alloco la memoria dinamicamente per i dati dell'immagine bitmap
	bitmapImage = new UCHAR[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// mi sposto all'inizio dei dati dell'immagine bitmap
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// leggo i dati dell'immagine bitmap
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close 
	errore = fclose(filePtr);
	if (errore != 0)
	{
		return false;
	}

	// inizializzo la posizione nell buffer image data
	int k = 0;

	// leggo i dati dell'immagine nell'array heigh map

	for (int j = 0; j<terrainHeight; j++)
	{
		for (int i = 0; i<terrainWidth; i++)
		{
			//le bitmaps sono al contrario quindi carico dal basso verso l'altoi nel bitmap array
			index = (terrainWidth * (terrainHeight - 1 - j)) + i;

			//leggo il valore pixel in scala di grigi dal bitmap image data in questa posizione
			height = bitmapImage[k];

			// immagazzino il valore pixel come altezza in questo punto nel height map array
			heightMap[index].pos.y = (float)height;

			// incremento l'indice del bitmap image data
			k += 3;
		}

		//Compenso per gli extra byte alla fine di ogni linea( non-divide by 2 bitmaps )
		k++;
	}

	//rilascio
	delete[] bitmapImage;
	bitmapImage = 0;

	// rilascio
	delete[] terrainFilename;
	terrainFilename = 0;

	return true;
}
void Geometrie::ShutdownHeightMap()
{
	// Rilascio
	if (heightMap)
	{
		delete[] heightMap;
		heightMap = 0;
	}

}
void Geometrie::SetTerrainCoordinates()
{
	int i, j, index;


	// aggiusto le coordinate di ogni elemento nell'array heightmap
	for (j = 0; j<terrainHeight; j++)
	{
		for (i = 0; i<terrainWidth; i++)
		{
			index = (terrainWidth * j) + i;

			// Setto le coordinate X e Z
			heightMap[index].pos.x = (float)i;
			heightMap[index].pos.z = -(float)j;

			// sposto la profondità del terreno nel range positivo F per esempio da (0, -256) a (256, 0)
			heightMap[index].pos.z += (float)(terrainHeight - 1);

			// Scalo l'altezza
			heightMap[index].pos.y /= heightScale;
		}
	}

}
bool Geometrie::BuildTerrainModel(MeshData& data)
{
	int index, index1, index2, index3, index4;

	int vertexCount = (terrainHeight - 1) * (terrainWidth - 1) * 6;


	data.Vertices.resize(vertexCount);
	
	
	index = 0;


	for (int j = 0; j<(terrainHeight - 1); j++)
	{
		for (int i = 0; i<(terrainWidth - 1); i++)
		{
			// Setto gli indici dei 4 punti del quadrato
			index1 = (terrainWidth * j) + i;          // alto sinistra
			index2 = (terrainWidth * j) + (i + 1);      // alto destra
			index3 = (terrainWidth * (j + 1)) + i;      // basso sinistra
			index4 = (terrainWidth * (j + 1)) + (i + 1);  // basso destra

			// Creo i triangoli del quadrato

			// Triangolo 1 - alto sinistra
			data.Vertices[index].Position.x = heightMap[index1].pos.x;
			data.Vertices[index].Position.y = heightMap[index1].pos.y;
			data.Vertices[index].Position.z = heightMap[index1].pos.z;
			data.Vertices[index].TexC.x = 0.0f;
			data.Vertices[index].TexC.y = 0.0f;
			data.Vertices[index].Normal = heightMap[index1].n;
			data.Vertices[index].Color = heightMap[index1].map;

			index++;

			// Triangolo 1 - alto destra
			data.Vertices[index].Position.x = heightMap[index2].pos.x;
			data.Vertices[index].Position.y = heightMap[index2].pos.y;
			data.Vertices[index].Position.z = heightMap[index2].pos.z;
			data.Vertices[index].TexC.x = 1.0f;
			data.Vertices[index].TexC.y = 0.0f;
			data.Vertices[index].Normal = heightMap[index2].n;
			data.Vertices[index].Color = heightMap[index2].map;
			index++;

			// Triangolo 1 - basso sinistra
			data.Vertices[index].Position.x = heightMap[index3].pos.x;
			data.Vertices[index].Position.y = heightMap[index3].pos.y;
			data.Vertices[index].Position.z = heightMap[index3].pos.z;
			data.Vertices[index].TexC.x = 0.0f;
			data.Vertices[index].TexC.y = 1.0f;
			data.Vertices[index].Normal = heightMap[index3].n;
			data.Vertices[index].Color = heightMap[index2].map;
			index++;

			// Triangolo 2 - basso sinistra
			data.Vertices[index].Position.x = heightMap[index3].pos.x;
			data.Vertices[index].Position.y = heightMap[index3].pos.y;
			data.Vertices[index].Position.z = heightMap[index3].pos.z;
			data.Vertices[index].TexC.x = 0.0f;
			data.Vertices[index].TexC.y = 1.0f;
			data.Vertices[index].Normal = heightMap[index3].n;
			data.Vertices[index].Color = heightMap[index3].map;
			index++;

			// Triangolo 2 - alto destra
			data.Vertices[index].Position.x = heightMap[index2].pos.x;
			data.Vertices[index].Position.y = heightMap[index2].pos.y;
			data.Vertices[index].Position.z = heightMap[index2].pos.z;
			data.Vertices[index].TexC.x = 1.0f;
			data.Vertices[index].TexC.y = 0.0f;
			data.Vertices[index].Normal = heightMap[index2].n;
			data.Vertices[index].Color = heightMap[index2].map;
			index++;

			// Triangolo 2 - basso destra
			data.Vertices[index].Position.x = heightMap[index4].pos.x;
			data.Vertices[index].Position.y = heightMap[index4].pos.y;
			data.Vertices[index].Position.z = heightMap[index4].pos.z;
			data.Vertices[index].TexC.x = 1.0f;
			data.Vertices[index].TexC.y = 1.0f;
			data.Vertices[index].Normal = heightMap[index4].n;
			data.Vertices[index].Color = heightMap[index4].map;
			index++;
		}
	}

	return true;

}
bool Geometrie::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	XMFLOAT3* normals;


	// Create a temporary array to hold the face normal vectors.
	normals = new XMFLOAT3[(terrainHeight - 1) * (terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(terrainHeight - 1); j++)
	{
		for (i = 0; i<(terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * terrainWidth) + i;          // Upper left vertex.

														// Get three vertices from the face.
			vertex1[0] = heightMap[index1].pos.x;
			vertex1[1] = heightMap[index1].pos.y;
			vertex1[2] = heightMap[index1].pos.z;

			vertex2[0] = heightMap[index2].pos.x;
			vertex2[1] = heightMap[index2].pos.y;
			vertex2[2] = heightMap[index2].pos.z;

			vertex3[0] = heightMap[index3].pos.x;
			vertex3[1] = heightMap[index3].pos.y;
			vertex3[2] = heightMap[index3].pos.z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j<terrainHeight; j++)
	{
		for (i = 0; i<terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i<(terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(terrainHeight - 1)))
			{
				index = (j * (terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (terrainWidth - 1)) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			heightMap[index].n.x = (sum[0] / length);
			heightMap[index].n.y = (sum[1] / length);
			heightMap[index].n.z = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true; 
}
// COLOR MAPS
bool Geometrie::LoadColorMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Apro la color map in modalità binaria
	error = fopen_s(&filePtr, colorMapFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Leggo il file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Leggo info header bitmap
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Mi assicuro che la dimensione della color map sia uguale alla dimensione del terreno per fare un facile  1 to 1 mapping

	if ((bitmapInfoHeader.biWidth != terrainWidth) || (bitmapInfoHeader.biHeight != terrainHeight))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  Since this is non-divide by 2 dimensions (eg. 257x257) need to add extra byte to each line.
	imageSize = terrainHeight * ((terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the color map portion of the height map structure.
	for (j = 0; j<terrainHeight; j++)
	{
		for (i = 0; i<terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (terrainWidth * (terrainHeight - 1 - j)) + i;

			heightMap[index].map.z = (float)bitmapImage[k] / 255.0f;
			heightMap[index].map.y = (float)bitmapImage[k + 1] / 255.0f;
			heightMap[index].map.x = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}

		// Compensate for extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Rilascio
	delete[] bitmapImage;
	bitmapImage = 0;

	// Rilascio
	delete[] colorMapFilename;
	colorMapFilename = 0;

	return true;
}