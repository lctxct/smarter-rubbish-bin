package main

import (
	"bufio"
	"context"
	"fmt"
	"log"
	"math"
	"os"
	"strings"

	firebase "firebase.google.com/go/v4"
	"firebase.google.com/go/v4/db"
	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
	"github.com/joho/godotenv"
	"google.golang.org/api/option"
)

var (
	ctx context.Context

	bot *tgbotapi.BotAPI

	client *db.Client

	keyboard = tgbotapi.NewReplyKeyboard(
		tgbotapi.NewKeyboardButtonRow(
			tgbotapi.NewKeyboardButton("/fill"),
		),
	)
)

func main() {
	var err error

	err = godotenv.Load(".env")
	if err != nil {
		log.Panic("Error loading .env file")
	}

	bot, err = tgbotapi.NewBotAPI(os.Getenv("BOT_API"))
	if err != nil {
		log.Panic(err)
	}

	// Set this to true to log all interactions with telegram servers
	bot.Debug = false

	u := tgbotapi.NewUpdate(0)
	u.Timeout = 60

	// Create a new cancellable background context. Calling `cancel()` leads to the cancellation of the context
	ctx = context.Background()
	ctx, cancel := context.WithCancel(ctx)

	initializeFirebase(ctx)

	// `updates` is a golang channel which receives telegram updates
	updates := bot.GetUpdatesChan(u)
	// Pass cancellable context to goroutine
	go receiveUpdates(ctx, updates)

	log.Println("Start listening for updates. Press enter to stop")

	// Wait for a newline symbol, then cancel handling updates
	bufio.NewReader(os.Stdin).ReadBytes('\n')
	cancel()

}

func initializeFirebase(ctx context.Context) {
	ao := map[string]interface{}{"uid": "smartapp"}
	conf := &firebase.Config{
		DatabaseURL:  os.Getenv("DATABASE_URL"),
		AuthOverride: &ao,
	}

	opt := option.WithCredentialsFile("secrets/smart-rubbish-bin-firebase-adminsdk.json")

	app, err := firebase.NewApp(ctx, conf, opt)
	if err != nil {
		log.Fatalln("Error initializing app:", err)
	}

	client, err = app.Database(ctx)
	if err != nil {
		log.Fatalln("Error initializing database client:", err)
	}
}

func receiveUpdates(ctx context.Context, updates tgbotapi.UpdatesChannel) {
	for {
		select {
		// stop looping if ctx is cancelled
		case <-ctx.Done():
			return
		// receive update from channel and then handle it
		case update := <-updates:
			handleUpdate(update)
		}
	}
}

func handleUpdate(update tgbotapi.Update) {
	switch {
	case update.Message != nil:
		handleMessage(update.Message)
		break
	}
}

func handleMessage(message *tgbotapi.Message) {
	user := message.From
	text := message.Text

	if user == nil {
		return
	}

	log.Printf("%s wrote %s", user.FirstName, text)

	var err error
	if strings.HasPrefix(text, "/") {
		err = handleCommand(message.Chat.ID, text)
	}

	if err != nil {
		log.Printf("An error occured: %s", err.Error())
	}
}

// When we get a command, we react accordingly
func handleCommand(chatId int64, command string) error {
	var err error

	switch command {
	case "/start":
		err = sendStart(chatId)
		break
	case "/fill":
		err = sendFill(chatId)
		break
	}

	return err
}

func sendStart(chatId int64) error {
	text := "Welcome!\n" +
		"I'm Philbot, and I'm here to provide some bin advice.\n\n" +
		"Available commands:\n" +
		"/fill - Check 🗑️ fill level"

	msg := tgbotapi.NewMessage(chatId, text)
	msg.ReplyMarkup = keyboard
	_, err := bot.Send(msg)
	return err
}

func sendFill(chatId int64) error {
	filltext := getFillText()

	msg := tgbotapi.NewMessage(chatId, filltext)
	msg.ParseMode = "MarkdownV2"
	_, err := bot.Send(msg)

	return err
}

func getFillText() string {
	fill := getFill()

	brow := int(math.Floor(fill / 20)) // rows of ■
	wrow := 5 - brow                   // rows of □
	bspare := int(math.Ceil(float64(int(math.Floor(fill))%20) / 2))
	var wspare int
	if bspare == 0 {
		wspare = 0
	} else {
		wspare = 10 - bspare
	}

	filltext := fmt.Sprintf("The 🗑️ is %.2f%% full.\n", fill)

	filldisplay := " ----------\n" +
		"|    ^_^   | ~ <3\n" +
		" ----------\n"

	// Build '□'
	for i := 0; i < wrow; i++ {
		filldisplay = filldisplay + "|□□□□□□□□□□|\n"
	}

	// Build spare '■' + □
	if wspare != 0 {
		filldisplay = filldisplay + "|"
		for i := 0; i < bspare; i++ {
			filldisplay = filldisplay + "■"
		}
		for i := 0; i < wspare; i++ {
			filldisplay = filldisplay + "□"
		}
		filldisplay = filldisplay + "|\n"
	}

	// Build '■'
	for i := 0; i < brow; i++ {
		filldisplay = filldisplay + "|■■■■■■■■■■|\n"
	}
	filldisplay = "```\n" + filldisplay + "\n```"

	fillmsg := escape(filltext+filldisplay, []string{"."})

	return fillmsg
}

func getFill() float64 {
	var tfill, rfill float64

	ref := client.NewRef("fill-level")
	var data map[string]interface{}
	if err := ref.Get(ctx, &data); err != nil {
		log.Fatalln("Error reading from database:", err)
	}

	if trashData, ok := data["trash"].(map[string]interface{}); ok {
		tfill, ok = trashData["level"].(float64)
		if !ok {
			fmt.Println("Trash level is not a float64")
		}
	}

	if recyclablesData, ok := data["recyclables"].(map[string]interface{}); ok {
		rfill, ok = recyclablesData["level"].(float64)
		if !ok {
			log.Println("Recyclables level is not a float64")
		}
	}

	if rfill > tfill {
		return rfill
	} else {
		return tfill
	}
}

func escape(text string, characters []string) string {
	for _, char := range characters {
		text = strings.ReplaceAll(text, char, "\\"+char)
	}
	return text
}
