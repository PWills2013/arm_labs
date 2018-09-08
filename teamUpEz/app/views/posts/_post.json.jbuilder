json.extract! post, :id, :title, :name, :mmr, :body, :created_at, :updated_at
json.url post_url(post, format: :json)