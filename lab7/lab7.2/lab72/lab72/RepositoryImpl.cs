using System.Net;
using System.Text.Json.Nodes;

namespace lab72;

[Obsolete("Obsolete")]
public class RepositoryImpl : IRepository
{
    // ReSharper disable once InconsistentNaming
    private const string CLIENT_URL = "https://dog.ceo/api/breeds/image/random";

    // ReSharper disable once InconsistentNaming
    private const string IMAGE_SAVE_FOLDER = @"C:\study\3course\Parallel Programming\lab7.2\lab72\lab72\bin\Debug\net8.0\images\";
    
    public async Task<List<string>> GetImageUrlsAsync(int imageNumber)
    {
        var listImageUrls = new List<string>(new string[imageNumber]);
        await Parallel.ForAsync(0, imageNumber, async (i, token) =>
        {
            var request = (HttpWebRequest)WebRequest.Create(CLIENT_URL);
            var response = (HttpWebResponse)request.GetResponse();
            var stream = response.GetResponseStream();
            var reader = new StreamReader(stream);
            var json = reader.ReadLineAsync(token);
            await json;

            if (json.Result != null)
            {
                var jsonObject = JsonNode.Parse(json.Result);
                if (jsonObject!["status"]!.ToString() == "success")
                {
                    listImageUrls[i] = jsonObject["message"]!.ToString();
                }
                else
                {
                    Console.WriteLine(StringProvider.ERROR_GET_IMAGE_URL + i);
                }
            }
            else
            {
                Console.WriteLine(StringProvider.ERROR_GET_IMAGE_URL + i);
            }
        });

        return listImageUrls;
    }

    private static string GetFileNameFromUrl(string url)
    {
        var beginIndex = url.Length - 1;
        for (; beginIndex > 0; beginIndex--)
        {
            if (url[beginIndex] == '/')
            {
                break;
            }
        }

        beginIndex++;
        return url.Substring(beginIndex, url.Length - beginIndex);
    }
    
    private static void Download(string url, string fileName)
    {
        try
        {
            using var client = new WebClient();
            client.DownloadFile(url, IMAGE_SAVE_FOLDER + fileName);
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            throw;
        }
    }

    public async Task LoadImageAsync(string url)
    {
        var fileName = GetFileNameFromUrl(url);
        Console.WriteLine(StringProvider.MESSAGE_START_DOWNLOAD + fileName);
        await Task.Run(() => Download(url, fileName));
        Console.WriteLine(StringProvider.MESSAGE_END_DOWNLOAD + fileName);
    }

    public List<string> GetImageUrls(int imageNumber)
    {
        var listImageUrls = new List<string>(new string[imageNumber]);
        for (var i = 0; i < imageNumber; i++)
        {
            var request = (HttpWebRequest)WebRequest.Create(CLIENT_URL);
            var response = (HttpWebResponse)request.GetResponse();
            var stream = response.GetResponseStream();
            var reader = new StreamReader(stream);
            var json = reader.ReadLine();
            
            if (json != null)
            {
                var jsonObject = JsonNode.Parse(json);
                if (jsonObject!["status"]!.ToString() == "success")
                {
                    listImageUrls[i] = jsonObject["message"]!.ToString();
                }
                else
                {
                    Console.WriteLine(StringProvider.ERROR_GET_IMAGE_URL + i);
                }
            }
            else
            {
                Console.WriteLine(StringProvider.ERROR_GET_IMAGE_URL + i);
            }
        }

        return listImageUrls;
    }
    
    public void LoadImage(string url)
    {
        var fileName = GetFileNameFromUrl(url);
        Console.WriteLine(StringProvider.MESSAGE_START_DOWNLOAD + fileName);
        Download(url, fileName);
        Console.WriteLine(StringProvider.MESSAGE_END_DOWNLOAD + fileName);
    }
}