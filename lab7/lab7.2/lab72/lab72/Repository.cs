namespace lab72;

public interface IRepository
{ 
    Task<List<string>> GetImageUrlsAsync(int imageNumber);
    Task LoadImageAsync(string url);
    
    List<string> GetImageUrls(int imageNumber);
    void LoadImage(string url);
}