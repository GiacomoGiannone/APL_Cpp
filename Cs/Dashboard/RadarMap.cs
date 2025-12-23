using Microsoft.Maui.Graphics;

namespace Dashboard;

public class RadarMap : IDrawable
{
    // Riferimento ai dati dei giocatori (passato dalla MainPage)
    private Dictionary<uint, (float X, float Y)> _players;

    public RadarMap(Dictionary<uint, (float X, float Y)> players)
    {
        _players = players;
    }

    public void Draw(ICanvas canvas, RectF dirtyRect)
    {
        // 1. Sfondo del Radar (Nero stile terminale)
        canvas.FillColor = Colors.Black;
        canvas.FillRectangle(dirtyRect);

        // 2. Griglia verde (opzionale, fa scena)
        canvas.StrokeColor = Colors.DarkGreen.WithAlpha(0.5f);
        canvas.StrokeSize = 1;
        for (int i = 0; i < dirtyRect.Width; i += 50) canvas.DrawLine(i, 0, i, dirtyRect.Height);
        for (int i = 0; i < dirtyRect.Height; i += 50) canvas.DrawLine(0, i, dirtyRect.Width, i);

        // 3. Disegna i Giocatori
        // IMPORTANTE: Blocchiamo i dati mentre disegniamo per evitare crash
        lock (_players)
        {
            foreach (var player in _players)
            {
                float x = player.Value.X;
                float y = player.Value.Y;

                // --- CALIBRAZIONE ---
                // Se il gioco ha coordinate grandi (es. 0-2000), dividi per un fattore
                // Esempio: x / 2.0f se la mappa è grande. Per ora lo lasciamo 1:1.
                float screenX = x; 
                float screenY = y;

                // Colore diverso per il Player 0 (Tu?) e gli altri
                canvas.FillColor = (player.Key == 0) ? Colors.Cyan : Colors.Red;
                
                // Disegna il pallino (x, y, larghezza, altezza)
                canvas.FillCircle(screenX, screenY, 10);

                // Scrivi l'ID sopra il pallino
                canvas.FontColor = Colors.White;
                canvas.DrawString($"P{player.Key}", screenX, screenY - 15, HorizontalAlignment.Center);
            }
        }
    }
}