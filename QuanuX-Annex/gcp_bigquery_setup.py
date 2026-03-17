import argparse
import logging
from google.cloud import bigquery

# Set up logging matching QuanuX-Annex patterns
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger("quanux.gcp.bigquery")

def register_external_table(project_id: str, dataset_id: str, table_id: str, gcs_uri: str):
    """
    Registers a BigQuery External Table against a GCS bucket containing Parquet files.
    This exposes the historical query surface to the Python modeling tier without moving data.
    """
    try:
        client = bigquery.Client(project=project_id)
        
        # Ensure dataset exists
        dataset_ref = client.dataset(dataset_id)
        try:
            client.get_dataset(dataset_ref)
        except Exception:
            logger.info(f"Dataset {dataset_id} not found. Creating it.")
            dataset = bigquery.Dataset(dataset_ref)
            dataset.location = "US"
            client.create_dataset(dataset)

        table_ref = dataset_ref.table(table_id)
        
        # Configure the external data source
        external_config = bigquery.ExternalConfig("PARQUET")
        external_config.source_uris = [gcs_uri]
        external_config.autodetect = True # Enable Parquet schema auto-detection
        
        table = bigquery.Table(table_ref)
        table.external_data_configuration = external_config
        
        table = client.create_table(table, exists_ok=True)
        
        logger.info(f"Successfully registered external table {project_id}.{dataset_id}.{table_id} pointing to {gcs_uri}")
        
    except Exception as e:
        logger.error(f"Failed to register BigQuery external table: {e}")
        raise

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="QuanuX GCP BigQuery External Table Setup")
    parser.add_argument("--project", required=True, help="GCP Project ID")
    parser.add_argument("--dataset", default="quanux_historical", help="BigQuery Dataset Name")
    parser.add_argument("--table", default="market_ticks", help="External Table Name")
    parser.add_argument("--uri", required=True, help="GCS URI (e.g. gs://quanux-historical-lake/ingestion/*.parquet)")
    
    args = parser.parse_args()
    register_external_table(args.project, args.dataset, args.table, args.uri)
